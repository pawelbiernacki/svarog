/***************************************************************************
 *   Copyright (C) 2019 by Pawel Biernacki                                 *
 *   pawel.f.biernacki@gmail.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "svarog.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits>
#include <fstream>

using namespace svarog;

configuration optimizer::my_configuration;

std::vector<std::string> kuna_vocabulary;


optimizer::task::task(const std::string & new_server_name, int new_port, const std::string & pw, const action * a):
	sockfd{0},
	my_action{a},
	password{pw},
	server_name{new_server_name},
	port{new_port}
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0) {
		std::cerr << "error opening socket\n";
		throw std::runtime_error("error opening socket");
	}

	server = gethostbyname(server_name.c_str());

	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		std::cerr << "connection error\n";
		close(sockfd);
		throw std::runtime_error("connection error");
	}
}


optimizer::task::~task()
{
	close(sockfd);
}


int optimizer::task::write_and_run(const std::string & data)
{
	return write(sockfd, data.c_str(), data.size());
}


int optimizer::task::read_from_server()
{
	bzero(buffer,SVAROG_MAX_MESSAGE_FROM_SERVER);
   
	int n = read(sockfd, buffer, SVAROG_MAX_MESSAGE_FROM_SERVER-1);
 
	if (n < 0) 
	{
		std::cerr << "error reading\n";
		throw std::runtime_error("error reading from server");
	}
	return n;
}


void optimizer::clear_tasks()
{
	for (auto i(vector_of_tasks.begin()); i!=vector_of_tasks.end(); i++)
	{
		delete *i;
	}
	vector_of_tasks.clear();
}


char* kuna_completion_generator(const char* text, int state) {
  // This function is called with state=0 the first time; subsequent calls are
  // with a nonzero state. state=0 can be used to perform one-time
  // initialization for this completion session.
  static std::vector<std::string> matches;
  static size_t match_index = 0;

  if (state == 0) {
    // During initialization, compute the actual matches for 'text' and keep
    // them in a static vector.
    matches.clear();
    match_index = 0;

    // Collect a vector of matches: vocabulary words that begin with text.
    std::string textstr = std::string(text);
    for (auto word : kuna_vocabulary) {
      if (word.size() >= textstr.size() &&
          word.compare(0, textstr.size(), textstr) == 0) {
        matches.push_back(word);
      }
    }
  }

  if (match_index >= matches.size()) {
    // We return nullptr to notify the caller no more matches are available.
    return nullptr;
  } else {
    // Return a malloc'd char* for the match. The caller frees it.
    return strdup(matches[match_index++].c_str());
  }
}


char** kuna_completer(const char* text, int start, int end) 
{
	// Don't do filename completion even if our generator finds no matches.
	rl_attempted_completion_over = 1;

	// Note: returning nullptr here will make readline use the default filename
	// completer.
	return rl_completion_matches(text, kuna_completion_generator);
}



optimizer::optimizer(): a(v, w), vs(v, w, a), 
	initial_input{nullptr}, 
	initial_action{nullptr}, 
	initial_belief{nullptr},
	last_solution{std::numeric_limits<float>::lowest()},
	file_counter{1}
{
	rl_attempted_completion_function = kuna_completer;
}

optimizer::~optimizer()
{
	if (initial_belief)
	{
		delete initial_belief;
	}
	
	for (std::list<knowledge_action*>::iterator i(list_of_knowledge_actions.begin()); i!=list_of_knowledge_actions.end(); i++)
	{
		delete *i;
	}
	list_of_knowledge_actions.clear();
	
	for (std::list<knowledge_impossible*>::iterator i(list_of_knowledge_impossibles.begin()); i!=list_of_knowledge_impossibles.end(); i++)
	{
		delete *i;
	}
	list_of_knowledge_impossibles.clear();
	
	for (std::list<knowledge_payoff*>::iterator i(list_of_knowledge_payoffs.begin()); i!=list_of_knowledge_payoffs.end(); i++)
	{
		delete *i;
	}
	list_of_knowledge_payoffs.clear();
	
	for (auto i(list_of_knowledge_precalculated.begin()); i!=list_of_knowledge_precalculated.end(); ++i)
	{
		delete *i;
	}
	list_of_knowledge_precalculated.clear();
}

value * optimizer::get_value(const char * n)
{
	for (int i=my_stack.size()-1; i>=0; i--)
	{
		if (my_stack[i].get_name() == n && my_stack[i].get_mode()==value_or_something_else::VALUE)
		{
			return my_stack[i].get_value();
		}
	}
	return w.get_value(n);
}

variable * optimizer::get_variable(const char * n)
{
	//my_stack.report_kuna(std::cout);
	
	for (int i=my_stack.size()-1; i>=0; i--)
	{
		if (my_stack[i].get_name() == n && my_stack[i].get_mode()==value_or_something_else::VARIABLE)
		{
			return my_stack[i].get_variable();
		}
	}
	return v.get_variable(n);
}

void optimizer::add_knowledge_impossible(knowledge_impossible * i)
{
	if (i == NULL)
	{
		throw std::runtime_error("attempt to add null as knowledge impossible");
	}
	list_of_knowledge_impossibles.push_back(i);
}

void optimizer::add_knowledge_precalculated(knowledge_precalculated * i)
{
	if (i == nullptr)
	{
		throw std::runtime_error("attempt to add null as knowledge precalculated");
	}
	list_of_knowledge_precalculated.push_back(i);
}


		
void optimizer::add_knowledge_action(knowledge_action * a)
{
	if (a == NULL)
	{
		throw std::runtime_error("attempt to add null as knowledge action");
	}
	list_of_knowledge_actions.push_back(a);
}

void optimizer::add_knowledge_payoff(knowledge_payoff * p)
{
	if (p == NULL)
	{
		throw std::runtime_error("attempt to add null as knowledge payoff");
	}
	list_of_knowledge_payoffs.push_back(p);
}



void optimizer::add_command(command * c)
{
	if (c == NULL)
	{
		throw std::runtime_error("attempt to add null as command");
	}
	vector_of_commands.push_back(c);
}

void optimizer::get_input(std::map<variable*, value*> & m, bool & eof)
{
	char *buffer;
	
	do
	{
		buffer = readline("svarog> ");
		
		if (buffer != nullptr)
		{
			if (strlen(buffer) > 0) 
			{
				add_history(buffer);
			}			
		}
		else
		{
			std::cout << "eof\n";
			eof = true;
			return;
		}
		//std::cin.getline(buffer, 1000);

		/*
		if (std::cin.eof())	
		{
			std::cout << "eof\n";
			eof = true;
			return;
		}
		*/
		
	}
	while (false);
		
		// parse the buffer containing a list of input variables with values
		// 
	std::stringstream ss(buffer);
	std::string q;
		
		
	std::list<variable*>::iterator i(list_of_input_variables.begin());
		
	while (!ss.eof())
	{
		ss >> q;
		value * qv = w.get_value(q.c_str());
		if (qv == NULL)
		{
			std::cerr << "illegal value " << q << "\n";
			exit(-1);
		}
		if (i == list_of_input_variables.end())
		{
			/*
			std::cerr << "too many values (got " << q << ")";
			exit(-1);
			*/
			break;
		}			
		//std::cout << (*i)->get_name() << "=>" << q << "\n";
		m.insert(std::pair<variable*,value*>(*i, qv));
		i++;
	}
	
	free(buffer);
}

void optimizer::execute(const action * a)
{
	std::cout << "optimal action:\n";
	a->report_kuna(std::cout);
	std::cout << "\n";
}



void optimizer::print_initial_loop_message() const
{
	std::cout << "I expect the values of the variables: ";
	for (std::list<variable*>::const_iterator i(list_of_input_variables.begin());
		 i != list_of_input_variables.end(); i++)
	{
		std::cout << (*i)->get_name() << " ";
	}
	std::cout << "\n";
}

void optimizer::on_surprise(const belief & b1, const action & a, const visible_state & vs, belief & target)
{
	float f = get_consequence_probability(b1,a,vs);
	
	std::cerr << "result\n";
	vs.report_kuna(std::cerr);
	std::cerr << "its probability was " << std::showpoint << f << "\n";

	const std::list<state*> &l1(b1.get_visible_state().get_list_of_states());
	const std::list<state*> &l2(vs.get_list_of_states());
	
	for (std::list<state*>::const_iterator i(l1.begin()); i!=l1.end(); i++)
	{
		evaluating_expressions_mode = GET_POSSIBLE;
		get_possible_state1 = *i;
		get_possible_state2 = NULL;
		if ((*i)->get_possible())
		{
			for (std::list<state*>::const_iterator j(l2.begin()); j!=l2.end(); j++)
			{
				evaluating_expressions_mode = GET_POSSIBLE;
				get_possible_state2 = *j;
				if ((*j)->get_possible())
				{
					evaluating_expressions_mode = GET_MODEL;
					float p = get_model_probability(*i, a, *j);
					
					if (p > 0.0f)
					{
						std::cerr << "for ";
						(*i)->report_kuna(std::cerr);
						std::cerr << " and ";
						(*j)->report_kuna(std::cerr);
						std::cerr << " the model probability " << std::showpoint << p << "\n";
					}
				}
			}
		}
	}
	throw std::runtime_error("failed to interpret the result");
}


void optimizer::populate_belief_for_consequence(const belief & b1, const action & a, const visible_state & vs, belief & target)
{
	/*
	std::cout << "BELIEF ";
	b1.report_kuna(std::cout);
	std::cout << "\n";
	*/
	
	const std::list<state*> &l1(b1.get_visible_state().get_list_of_states());
	const std::list<state*> &l2(vs.get_list_of_states());

	std::map<state*,float> map_state_to_probability;
	float sum = 0.0;
	
	for (std::list<state*>::const_iterator j(l2.begin()); j!=l2.end(); j++)
	{
		map_state_to_probability.insert(std::pair<state*,float>(*j, 0.0));
	}
	
	for (std::list<state*>::const_iterator i(l1.begin()); i!=l1.end(); i++)
	{
		/*
		std::cout << "state1 ";
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
		*/
		evaluating_expressions_mode = GET_POSSIBLE;
		get_possible_state1 = *i;
		get_possible_state2 = NULL;
		if ((*i)->get_possible())
		{
			for (std::list<state*>::const_iterator j(l2.begin()); j!=l2.end(); j++)
			{
				/*
				std::cout << "state2 ";
				(*j)->report_kuna(std::cout);
				std::cout << "\n";
				*/
				evaluating_expressions_mode = GET_POSSIBLE;
				get_possible_state2 = *j;
				if ((*j)->get_possible())
				{
					evaluating_expressions_mode = GET_MODEL;
					
					if (b1.get_probability(*i)>0.0)
						map_state_to_probability[*j] += b1.get_probability(*i)*
							get_model_probability(*i, a, *j);
				}
			}
		}
	}
	
	// normalize the belief
	for (std::list<state*>::const_iterator j(l2.begin()); j!=l2.end(); j++)
	{
		evaluating_expressions_mode = GET_POSSIBLE;
		get_possible_state1 = *j;
		get_possible_state2 = *j;
		if ((*j)->get_possible())
			sum += map_state_to_probability[*j];
	}
	
	if (sum == 0.0)
	{
		//target.make_uniform();
		on_surprise(b1, a, vs, target);
	}
	else
	for (std::list<state*>::const_iterator j(l2.begin()); j!=l2.end(); j++)
	{
		evaluating_expressions_mode = GET_POSSIBLE;
		get_possible_state1 = *j;
		get_possible_state2 = *j;
		
		if ((*j)->get_possible())
			target.set_probability(*j, map_state_to_probability[*j]/sum);
		else
			target.set_probability(*j, 0.0);
		//std::cout << "for " << *j << " value = " << map_state_to_probability[*j] << "/" << sum << "\n";
	}
}



float optimizer::get_consequence_probability(const belief & b1, const action & a, const visible_state & vs)
{
	const std::list<state*> &l1(b1.get_visible_state().get_list_of_states());
	const std::list<state*> &l2(vs.get_list_of_states());
	float sum = 0.0;
	
	for (std::list<state*>::const_iterator i(l1.begin()); i!=l1.end(); i++)
	{
		evaluating_expressions_mode = GET_POSSIBLE;
		get_possible_state1 = *i;
		get_possible_state2 = NULL;
		
		if ((*i)->get_possible())
		{
			for (std::list<state*>::const_iterator j(l2.begin()); j!=l2.end(); j++)
			{
				evaluating_expressions_mode = GET_POSSIBLE;
				get_possible_state2 = *j;
				if ((*j)->get_possible())
				{
					evaluating_expressions_mode = GET_MODEL;
					sum += b1.get_probability(*i)*get_model_probability(*i, a, *j);
				}
			}
		}
	}
	
	return sum;
}


float optimizer::get_payoff_expected_value_for_consequences(const belief & b1, int n, const action & a)
{
	if (n <= 0)
		return 0.0;
	
	const std::list<visible_state*> &l(b1.get_visible_state().get_visible_states().get_list_of_visible_states());
	float sum = 0.0;
	for (std::list<visible_state*>::const_iterator i(l.begin()); i!=l.end(); i++)
	{
		/*
			std::cout << "for consequence " << *i << "\n";
			(*i)->report_kuna(std::cout);
			//std::cout << "probability " << get_consequence_probability(b1, a, **i) << "\n";
		*/
		
		float cp = get_consequence_probability(b1, a, **i);
		
		if (cp > 0.0)
		{
			belief b2(**i, *this);
			// populate the belief
			populate_belief_for_consequence(b1, a, **i, b2);
		
			// get optimal action
			const action * a1 = get_optimal_action(b2, n-1);
			
			sum += cp*((*i)->get_payoff()+get_payoff_expected_value_for_consequences(b2, n-1, *a1));
		}
	}
	return sum;
}


float optimizer::solve(int n)
{
	// identify the belief
	belief * b = initial_belief;
	
	if (!b)
	{
		throw std::runtime_error("use belief command to set the initial belief");
	}
	
	
	// identify the action
	const action *a = initial_action;
	
	if (!a)
	{
		throw std::runtime_error("use action command to set the initial action");
	}
	
	
	get_illegal_visible_state1 = &b->get_visible_state();
	evaluating_expressions_mode = GET_IS_ILLEGAL;
		
	if (b->get_visible_state().get_is_illegal(a))
		return std::numeric_limits<float>::lowest();
	
	float m = get_payoff_expected_value_for_consequences(*b, n, *a);
	return m;
}


std::string optimizer::get_partial_task_specification(const belief & b, int n, const action * a) const
{
	std::stringstream s;
	
	s 
	<< "values {\n"
	<< "value ";
	
	bool first = true;
	
	for (auto i(w.get_vector_of_values().begin()); i!=w.get_vector_of_values().end(); i++)
	{
		if (!first) s << ",";
		s << (*i)->get_name();
		first = false;
	}
	s 
	<< ";\n"
	<< "}\n";
	
	s 
	<< "variables {\n";
	for (auto i(v.get_vector_of_variables().begin()); i!=v.get_vector_of_variables().end(); i++)
	{
		s << (*i)->get_type_name() << " variable " << (*i)->get_name() << ":{";
		first = true;
		for (std::list<const value*>::const_iterator j((*i)->get_list_of_possible_values().begin());
			 j != (*i)->get_list_of_possible_values().end(); j++)
			 {
				 if (!first) s << ",";
				 s << (*j)->get_name();
				 first = false;
			 }
		s << "};\n";
	}
	
	s << "}\n";
	
	s << "knowledge {\n";
	
	for (auto i(list_of_knowledge_actions.begin()); i!= list_of_knowledge_actions.end(); i++)
	{
		(*i)->report_kuna(s);
	}
	
	for (auto i(f.get_vector_of_functions().begin()); i!=f.get_vector_of_functions().end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}

	for (auto i(list_of_knowledge_impossibles.begin()); i!=list_of_knowledge_impossibles.end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}

	for (auto i(list_of_knowledge_payoffs.begin()); i!=list_of_knowledge_payoffs.end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}
	
	s << "}\n";
	
	
	// pass input
	
	s << "input {";
	
	const visible_state & vs{b.get_visible_state()};
	
	first = true;
	for (auto i(v.get_vector_of_variables().begin()); i!=v.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_input_variable())
		{
			if (!first) s << ",";
			
			s << (*i)->get_name() << "=>";
			const value * my_value = vs.get_input_variable_value(*i);
			
			if (!my_value)
			{
				throw std::runtime_error("failed to find input variable value");
			}
			s << my_value->get_name();
			first = false;
		}
	}
	
	s << "};\n";
	
	
	// pass belief
	
	s << "belief {\n";
	for (auto i(vs.get_list_of_states().begin()); i!=vs.get_list_of_states().end(); i++)
	{
		if (b.get_probability(*i) == 0.0)
			continue;
		
		s << "case {";
		first = true;
		for (auto j(v.get_vector_of_variables().begin()); j != v.get_vector_of_variables().end(); j++)
		{
			if ((*j)->get_is_hidden_variable())
			{
				if (!first) s << ",";
				s << (*j)->get_name() << "=>" << (*i)->get_hidden_variable_value(*j)->get_name() << " ";
				first = false;
			}
			else
			if ((*j)->get_is_input_variable())
			{
				if (!first) s << ",";
				s << (*j)->get_name() << "=>" << vs.get_input_variable_value(*j)->get_name() << " ";
				first = false;
			}
		}
		
		s << "}";
		
		s << ":" << std::dec << std::showpoint << b.get_probability(*i) << ";\n";
	}
	
	s << "};\n";
	
	s << "action {";
	first = true;
	for (auto j(v.get_vector_of_variables().begin()); j != v.get_vector_of_variables().end(); j++)
	{
		if ((*j)->get_is_output_variable())
		{
			if (!first) s << ",";
			s << (*j)->get_name() << "=>" << a->get_output_variable_value(*j)->get_name() << " ";
			first = false;
		}
	}
	s << "};\n";
	
	s << "solve(" << n << ");\n";
	
	return s.str();
}


const action * optimizer::get_optimal_action_using_servers(const belief & b, int n)
{
	float max = std::numeric_limits<float>::lowest();
	const action * argmax = nullptr;
	try
	{
		int server_index = 0;
		std::list<action*>::const_iterator i(a.get_list_of_actions().begin());
	
		for (;i != a.get_list_of_actions().end(); i++, server_index++)
		{
			if (server_index == my_configuration.get_amount_of_servers())
			{
				server_index = 0;
			}
			task * t = new task(
				my_configuration.get_server_name(server_index), my_configuration.get_server_port(server_index), my_configuration.get_server_password(server_index), *i);
	
			add_task(t);
		}
	}
	catch (...)
	{
		std::cerr << "error connecting server\n";
		throw;
	}
	
	// run the tasks
	char buffer[1024];
	for (auto i(vector_of_tasks.begin()); i!=vector_of_tasks.end(); i++)
	{
		std::stringstream s;
		s << "#!svarog " << (*i)->get_password() << "\n";
		
		s << get_partial_task_specification(b, n, (*i)->get_action());
		
		//std::cout << s.str();
		
		std::stringstream specification_stream(s.str());
		
		memset(buffer, 0, 1024);
		while (specification_stream.read(buffer, 1023))
		{
			//std::cout << line << "\n";
			(*i)->write_and_run(std::string(buffer, strlen(buffer)));
			memset(buffer, 0, 1024);
		}
		if (specification_stream.eof())
		{
			(*i)->write_and_run(std::string(buffer, strlen(buffer)));
		}
		
		(*i)->write_and_run("#END");
	}
	
	//std::cout << "all tasks have been assigned\n";
	
	// collect the results
	for (auto i(vector_of_tasks.begin()); i!=vector_of_tasks.end(); i++)
	{
		std::string response = (*i)->get_response();
		std::cout << response << " ";
		(*i)->get_action()->report_kuna(std::cout);
		std::cout << " " << (*i)->get_server_name() << ":" << (*i)->get_port();
		std::cout << "\n";
		
		std::stringstream response_stream(response);

		float m = 0.0;
		
		response_stream >> m;
				
		if (!response_stream.bad() && (m > max || argmax==nullptr))
		{
			max = m;
			argmax = (*i)->get_action();
		}
	}
	clear_tasks();
	return argmax;
}



const action * optimizer::get_optimal_action(const belief & b, int n)
{
	float max = std::numeric_limits<float>::lowest();
	const action * argmax = nullptr;
	
	
	// try to use the precalculated knowledge
	for (auto i(list_of_knowledge_precalculated.begin());
		 i != list_of_knowledge_precalculated.end(); ++i)
	{
		if ((*i)->get_depth()>=n)
		{
			// this searching could be improved by using a map
			auto & mylist((*i)->get_list_of_objects_on_visible_state());
			for (auto j(mylist.begin()); j!=mylist.end(); ++j)
			{
				const query * q = (*j)->get_query();
				if (b.get_visible_state().get_match(*q))
				{
					float distance = std::numeric_limits<float>::max();
					auto & mylist_on_belief((*j)->get_list_of_objects_on_belief());
					for (auto k(mylist_on_belief.begin()); k!=mylist_on_belief.end(); ++k)
					{
						float d = (*k)->get_distance(b);
						if (d < distance)
						{
							distance = d;
							
							// this could be done faster
							argmax = a.get(*(*k)->get_action_query());
						}
					}
					
					if (argmax)
					{
						std::cout << "got precalculated action\n";
						return argmax;
					}
					break;
				}
			}
		}
	}
	
	
	std::list<action*>::const_iterator i(a.get_list_of_actions().begin());
	
	for (;i != a.get_list_of_actions().end(); i++)
	{
		get_illegal_visible_state1 = &b.get_visible_state();
		evaluating_expressions_mode = GET_IS_ILLEGAL;
		
		if (b.get_visible_state().get_is_illegal(*i))
			continue;
				
		float m = get_payoff_expected_value_for_consequences(b, n, **i);
		
		if (m > max || argmax==NULL)
		{
			max = m;
			argmax = *i;
		}
	}
	
	return argmax;
}

float optimizer::get_model_probability(const state * s1, const action &a, const state * s2)
{
	get_model_state1 = s1;
	get_model_state2 = s2;
	
	/*
	std::cout << "get_model_probability for ";
	s1->report_kuna(std::cout);
	std::cout << " and state ";
	s2->report_kuna(std::cout);
	*/
	for (std::list<knowledge_action*>::const_iterator i(list_of_knowledge_actions.begin()); i!=list_of_knowledge_actions.end(); i++)
	{	
		if (a.get_match(*(*i)->get_query()))
		{
			/*
			std::cout << "action ";
			a.report_kuna(std::cout);
			std::cout << " matching ";
			(*i)->get_query()->report_kuna(std::cout);
			std::cout << "\n";
			*/
			const std::list<knowledge_action::knowledge_case*> & l((*i)->get_list_of_cases());

			/*
			for (std::list<knowledge_action::knowledge_case*>::const_iterator j(l.begin()); j!=l.end(); j++)
			{
				if ((*j)->get_contains_illegal())
				{
					std::cout << "skip case " << (*j)->get_name() << "\n";
					continue;
				}
				std::cout << "consider case " << (*j)->get_name() << "\n";
			}
			*/
			
			for (std::list<knowledge_action::knowledge_case*>::const_iterator j(l.begin()); j!=l.end(); j++)
			{
				if ((*j)->get_contains_illegal())
				{
					//std::cout << "skip case " << (*j)->get_name() << "\n";
					continue;
				}
				
				bool found = true;
				float f = get_model_probability_for_case(*j, found);
								
				if (found)
				{
					//std::cout << "matched! " << std::showpoint << f << "\n";
					return f;
				}
			}
			
			//std::cout << "finished the cases\n";
		}
	}
	
	return 0.0f;
}

float optimizer::get_model_probability_for_case(knowledge_action::knowledge_case * j, bool & found)
{		
	//std::cout << "case " << j->get_name() << "\n";
	const std::list<clause*> &k(j->get_list_of_clauses());
				
	for (std::list<clause*>::const_iterator m(k.begin()); m!=k.end(); m++)
	{
		if (!(*m)->get_match(*this))
		{
			/*
			std::cout << "failed for clause ";
			(*m)->report_kuna(std::cout);
			std::cout << "\n";
			*/
			found = false;
			break;
		}
	}
	
	if (found)
	{
		const std::list<knowledge_action::knowledge_case::knowledge_probability_case*> & n(j->get_list_of_probability_cases());
		
		for (std::list<knowledge_action::knowledge_case::knowledge_probability_case*>::const_iterator o(n.begin()); o!=n.end(); o++)
		{
			//std::cout << "consider probability case " << (*o)->get_name() << "\n";
			
			// if all clauses matched - return the probability
			const std::list<clause*> &q((*o)->get_list_of_clauses());
			
			bool found_probability = true;
			for (std::list<clause*>::const_iterator m(q.begin()); m!=q.end(); m++)
			{
				if (!(*m)->get_match(*this))
				{
					found_probability = false;
					break;
				}
			}
			
			if (found_probability)
			{
				//std::cout << "found probability " << std::showpoint << (*o)->get_probability() << "\n";
				return (*o)->get_probability();
			}
			else
			{
				//std::cout << "not found probability\n";
			}
		}
	}
	
	return 0.0f;
}


void optimizer::set_apriori_belief(belief * b) const
{
	b->make_uniform();	// the uniform distribution is the default a-priori distribution
						// redefine the function to change it
						// in the redefined version please take a look at the visible state of the belief
	/*
	std::cout << "apriori belief ";
	b->report_kuna(std::cout);
	std::cout << "\n";
	*/
}

void optimizer::print_belief(const belief & b) const
{
	b.report_kuna(std::cout);
}

void optimizer::declare_values()
{
	mode = DECLARING_VALUES;
}

void optimizer::declare_variables_values()
{
	mode = DECLARING_VARIABLES_VALUES;
}

void optimizer::add_value(const char * s)
{
	value * f;
	
	switch (mode)
	{
		case DECLARING_VALUES: 
			
			// check for double values
			for (int i=0; i<w.get_vector_of_values().size(); i++)
			{
				if (w.get_vector_of_values()[i]->get_name() == s)
				{
					std::stringstream t;
					t << "double value " << s;
					throw std::runtime_error(t.str());
				}
			}
			
			
			//std::cout << "add global value " << s << "\n"; 
			f = new value(s);
			w << f;
			
			kuna_vocabulary.push_back(std::string(s));
			
			break;
		case DECLARING_VARIABLES_VALUES: 
			
			f = NULL;
			for (int i=0; i<w.get_vector_of_values().size(); i++)
			{
				if (w.get_vector_of_values()[i]->get_name() == s)
				{
					f = w.get_vector_of_values()[i];
				}
			}
			
			//std::cout << "add variable value " << s << "\n";
			
			if (f == NULL)
			{
				std::stringstream t;
				t << "value " << s << " not found";
				throw std::runtime_error(t.str());
			}
			else
			{
				if (e->get_can_have_value(f))
				{
					std::stringstream t;
					t << "duplicate value " << s;
					throw std::runtime_error(t.str());
				}
				e->add_possible_value(f);
			}
			
			break;
	}
}


void optimizer::create_input_variable(const char * s)
{
	// check for double variables
	for (int i=0; i<v.get_vector_of_variables().size(); i++)
	{
		if (v.get_vector_of_variables()[i]->get_name() == s)
		{
			std::stringstream t;
			t << "double variable " << s;
			throw std::runtime_error(t.str());
		}
	}
	
	e = new input_variable(s);
	v << e;
}

void optimizer::create_hidden_variable(const char * s)
{
	// check for double variables
	for (int i=0; i<v.get_vector_of_variables().size(); i++)
	{
		if (v.get_vector_of_variables()[i]->get_name() == s)
		{
			std::stringstream t;
			t << "double variable " << s;
			throw std::runtime_error(t.str());
		}
	}

	e = new hidden_variable(s);
	v << e;
}

void optimizer::create_output_variable(const char * s)
{
	// check for double variables
	for (int i=0; i<v.get_vector_of_variables().size(); i++)
	{
		if (v.get_vector_of_variables()[i]->get_name() == s)
		{
			std::stringstream t;
			t << "double variable " << s;
			throw std::runtime_error(t.str());
		}
	}

	e = new output_variable(s);
	v << e;
}

void optimizer::make_model()
{	
	a.populate();
	vs.populate(*this);
}

void optimizer::calculate_payoff()
{
	vs.calculate_payoff(*this);
}


void optimizer::run()
{
	for (std::vector<command*>::const_iterator i(vector_of_commands.begin()); i!=vector_of_commands.end(); i++)
	{
		(*i)->execute(*this);
	}
}


bool optimizer::get_contains_interactive_commands() const
{
	for (std::vector<command*>::const_iterator i(vector_of_commands.begin()); i!=vector_of_commands.end(); i++)
	{
		if ((*i)->get_is_interactive())
		{
			return true;
		}
	}
	return false;
}

void optimizer::set_initial_belief(belief * b)
{
	if (initial_belief)
	{
		delete initial_belief;
	}
	initial_belief = b;
}


extern "C" void kuna_open_file(void * scanner, const char * f);
extern "C" void kuna_close_file(void * scanner);
extern "C" void svaroglex_init(void **);
extern "C" void svaroglex_destroy(void *);


typedef void* yyscan_t;

typedef struct yy_buffer_state * YY_BUFFER_STATE;

extern "C" YY_BUFFER_STATE svarog_scan_string (char *yy_str ,yyscan_t yyscanner );
extern "C" void svarog_delete_buffer (YY_BUFFER_STATE b ,yyscan_t yyscanner );

struct class_with_scanner
{
	void * scanner;
	void * optimizer;
};

int kuna_cpp_parse_error_to_stderr(class_with_scanner *);
int kuna_cpp_parse_error_to_stream(class_with_scanner *, std::ostream * s);

int optimizer::parse(const char * filename)
{
	class_with_scanner c;
	c.optimizer = this;

	svaroglex_init ( &c.scanner );
	kuna_open_file(c.scanner, filename);

	int i = kuna_cpp_parse_error_to_stderr(&c);

	if (i != 0)
		std::cout << "error" << "\n";
	
	kuna_close_file(c.scanner);
	svaroglex_destroy (c.scanner);

	return i;
}


int optimizer::parse_buffer(const char * buffer)
{
	std::stringstream stream;
	class_with_scanner c;
	c.optimizer = this;

	svaroglex_init ( &c.scanner );
	YY_BUFFER_STATE b = svarog_scan_string(const_cast<char*>(buffer), c.scanner);

	int i = kuna_cpp_parse_error_to_stream(&c, &stream);

	svarog_delete_buffer(b, c.scanner);
	svaroglex_destroy (c.scanner);

	set_error_message(stream.str());
	
	return i;	
}
