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
using namespace svarog;

visible_state::~visible_state()
{
	for (std::list<state*>::const_iterator i(list_of_states.begin()); i!=list_of_states.end(); i++)
	{
		delete *i;
	}
	list_of_states.clear();
}


state * visible_state::get(const std::map<variable*, value*> & m)
{
	for (std::list<state*>::iterator i(list_of_states.begin()); i!=list_of_states.end(); i++)
	{
		if ((*i)->get_match(m))
			return *i;
	}
	return NULL;
}

const state * visible_state::get(const std::map<variable*, value*> & m) const
{
	for (std::list<state*>::const_iterator i(list_of_states.begin()); i!=list_of_states.end(); i++)
	{
		if ((*i)->get_match(m))
			return *i;
	}
	return NULL;
}


bool visible_state::get_match(const std::map<variable*, value*> & m) const
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_input_variable())
		{
			if (map_input_variable_to_value.at(*i) != m.at(*i))
				return false;
		}
	}
	return true;
}


void visible_state::report_kuna(std::ostream & s) const
{
	bool first = true;
	s << "{";
	
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_input_variable())
		{
			if (!first) s << ",";
			s << (*i)->get_name() << "=>" << map_input_variable_to_value.at(*i)->get_name() << " ";
			first = false;
		}
	}
	s << "}";
	//s << "contains " << list_of_states.size() << " states\n";
	
}



float visible_state::get_payoff() const
{
	return my_visible_states.get_payoff(this);
}

bool visible_state::get_allowed(const std::map<variable*, std::vector<value*>::const_iterator> & m) const
{
	for (std::vector<variable*>::iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_hidden_variable())
		{
			if (!(*i)->get_can_have_value(*m.at(*i)))
			{
				return false;
			}
		}
	}

	return true;
}



void visible_state::populate()
{
	std::vector<variable*> vector_of_hidden_variables;
	std::map<variable*, std::vector<value*>::const_iterator> map_hidden_variable_to_value;

	for (std::vector<variable*>::iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_hidden_variable())
		{
			//std::cout << (*i)->get_name() << " is hidden variable\n";
			vector_of_hidden_variables.push_back(*i);
			map_hidden_variable_to_value.insert(std::pair<variable*,std::vector<value*>::iterator>(*i, my_values.get_vector_of_values().begin()));
		}
	}
	

	while (true)
	{
		state * s;
		
		if (get_allowed(map_hidden_variable_to_value)) 
		{
			list_of_states.push_back(s = new state(my_variables, my_values, *this, my_actions, my_optimizer));

			for (int j=0; j<vector_of_hidden_variables.size(); j++)
			{
				s->insert(vector_of_hidden_variables[j], *map_hidden_variable_to_value[vector_of_hidden_variables[j]]);
			}
		}
		
		//s->report(std::cout);
		
		// increment all hidden variables
		int i=vector_of_hidden_variables.size()-1;
		while (i>=0)
		{
			//std::cout << "increment " << vector_of_hidden_variables[i]->get_name() << "\n";
			map_hidden_variable_to_value[vector_of_hidden_variables[i]]++;
			
			if (map_hidden_variable_to_value[vector_of_hidden_variables[i]] == my_values.get_vector_of_values().end())
			{
				map_hidden_variable_to_value[vector_of_hidden_variables[i]] = my_values.get_vector_of_values().begin();
				i--;
				continue;
			}
			else
				break;
		}
		
		if (i<0)
			break;
	}
}

bool visible_state::get_is_illegal_for_case(const action * a, knowledge_action::knowledge_case * j) const
{
	//std::cout << "consider case " << j->get_name() << "\n";
	if (!j->get_contains_illegal())
		return false;
	
	const std::list<clause*> &k(j->get_list_of_clauses());

	bool found = true;
	bool is_illegal = false;
	for (std::list<clause*>::const_iterator m(k.begin()); m!=k.end(); m++)
	{
		if (!(*m)->get_match(my_optimizer))
		{
			found = false;
			break;
		}
		if ((*m)->get_is_illegal())
		{
			is_illegal = true;
		}
	}
	
	if (found && is_illegal)
	{
		//std::cout << "matched is illegal for case " << j->get_name() << "\n";
	}
	
	return found && is_illegal;
}

bool visible_state::get_is_illegal(const action * a) const
{
	for (std::list<knowledge_action*>::const_iterator i(my_optimizer.list_of_knowledge_actions.begin()); i!= my_optimizer.list_of_knowledge_actions.end(); i++)
	{
		if (a->get_match(*(*i)->get_query()))
		{
			//std::cout << "consider knowledge action\n";
			//(*i)->report_kuna(std::cout);
			
			
			const std::list<knowledge_action::knowledge_case*> & l((*i)->get_list_of_cases());
			
			for (std::list<knowledge_action::knowledge_case*>::const_iterator j(l.begin()); j!=l.end(); j++)
			{
				if (get_is_illegal_for_case(a, *j))
				{
					return true;
				}
			}
		}
	}
	return false;
}




int visible_state::get_amount_of_possible_states() const
{
	int amount = 0;
	for (std::list<state*>::const_iterator i(list_of_states.begin()); i!=list_of_states.end(); i++)
	{
		my_optimizer.evaluating_expressions_mode = optimizer::GET_POSSIBLE;
		my_optimizer.get_possible_state1 = *i;
		my_optimizer.get_possible_state2 = NULL;
		
		if ((*i)->get_possible())
			amount++;
	}
	return amount;
}
