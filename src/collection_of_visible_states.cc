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


bool collection_of_visible_states::get_allowed(const std::map<variable*, std::vector<value*>::const_iterator> & m) const
{
	for (std::vector<variable*>::iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_input_variable())
		{
			if (!(*i)->get_can_have_value(*m.at(*i)))
			{
				return false;
			}
		}
	}

	return true;
}

void collection_of_visible_states::calculate_payoff(optimizer & o)
{
	//std::cout << "calculating payoff\n";
	
	for (std::list<visible_state*>::iterator i(list_of_visible_states.begin()); i!=list_of_visible_states.end(); i++)
	{
		o.get_payoff_visible_state1 = *i;
		o.evaluating_expressions_mode = optimizer::GET_PAYOFF;
	
		/*
		std::cout << "calculating payoff for ";
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
		*/
		
		for (std::list<knowledge_payoff*>::iterator j(o.list_of_knowledge_payoffs.begin()); j!=o.list_of_knowledge_payoffs.end(); j++)
		{
			if ((*j)->get_match(o))
			{
				set_payoff(*i, (*j)->get_payoff());
				/*
				std::cout << "payoff for ";
				(*i)->report_kuna(std::cout);
				std::cout << " equals " << (*j)->get_payoff() << "\n";
				*/
			}
		}
	}
}


void collection_of_visible_states::populate(optimizer & o)
{
	std::map<variable*, std::vector<value*>::const_iterator> map_input_variable_to_value;
	std::vector<variable*> vector_of_input_variables;
	
	for (std::vector<variable*>::iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_input_variable())
		{
			//std::cout << (*i)->get_name() << " is input variable\n";
			vector_of_input_variables.push_back(*i);
			map_input_variable_to_value.insert(std::pair<variable*,std::vector<value*>::iterator>(*i, my_values.get_vector_of_values().begin()));
		}
	}
	
	while (true)
	{
		visible_state * s;
		
		if (get_allowed(map_input_variable_to_value))
		{
			list_of_visible_states.push_back(s = new visible_state(my_variables, my_values, my_actions, *this, o));
			s->populate();

			for (int j=0; j<vector_of_input_variables.size(); j++)
			{
				s->insert(vector_of_input_variables[j], *map_input_variable_to_value[vector_of_input_variables[j]]);
			}
		}
		
		//s->report(std::cout);
		
		// increment the input variables
		int i=vector_of_input_variables.size()-1;
		while (i>=0)
		{
			//std::cout << "increment " << vector_of_input_variables[i]->get_name() << "\n";
			map_input_variable_to_value[vector_of_input_variables[i]]++;
			
			if (map_input_variable_to_value[vector_of_input_variables[i]] == my_values.get_vector_of_values().end())
			{
				map_input_variable_to_value[vector_of_input_variables[i]] = my_values.get_vector_of_values().begin();
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




collection_of_visible_states::~collection_of_visible_states()
{
	for (std::list<visible_state*>::iterator i(list_of_visible_states.begin()); i!=list_of_visible_states.end(); i++)
	{
		delete *i;
	}
	list_of_visible_states.clear();
}


collection_of_visible_states::collection_of_visible_states(collection_of_variables & v, collection_of_values & w, 
	collection_of_actions & a): 
	my_variables(v),
	my_values(w),
	my_actions(a)
{
	
}



visible_state * collection_of_visible_states::get(const std::map<variable*, value*> & m)
{
	for (std::list<visible_state*>::iterator i(list_of_visible_states.begin()); i!=list_of_visible_states.end(); i++)
	{
		if ((*i)->get_match(m))
			return *i;
	}
	return NULL;
}


const visible_state* collection_of_visible_states::get(const std::map< variable*, value* >& m) const
{
	for (std::list<visible_state*>::const_iterator i(list_of_visible_states.begin()); i!=list_of_visible_states.end(); i++)
	{
		if ((*i)->get_match(m))
			return *i;
	}
	return NULL;
}


void collection_of_visible_states::report_kuna(std::ostream & s) const
{
	for (std::list<visible_state*>::const_iterator i(list_of_visible_states.begin());
		 i!=list_of_visible_states.end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
		//s << "payoff " << std::showpoint << map_visible_state_to_payoff_value.at(*i) << "\n\n";
	}
}
