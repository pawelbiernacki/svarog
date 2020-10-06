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

bool collection_of_actions::get_allowed(const std::map<variable*, std::vector<value*>::const_iterator> & m) const
{
	for (std::vector<variable*>::iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_output_variable())
		{
			if (!(*i)->get_can_have_value(*m.at(*i)))
			{
				return false;
			}
		}
	}
	return true;
}

void collection_of_actions::populate()
{
	std::vector<variable*> vector_of_output_variables;
	std::map<variable*, std::vector<value*>::const_iterator> map_output_variable_to_value;

	for (std::vector<variable*>::iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_output_variable())
		{
			//std::cout << (*i)->get_name() << " is hidden variable\n";
			vector_of_output_variables.push_back(*i);
			map_output_variable_to_value.insert(std::pair<variable*,std::vector<value*>::iterator>(*i, my_values.get_vector_of_values().begin()));
		}
	}
	
	while (true)
	{
		action * a;
		if (get_allowed(map_output_variable_to_value))
		{
			list_of_actions.push_back(a = new action(my_variables, my_values));

			for (int j=0; j<vector_of_output_variables.size(); j++)
			{
				a->insert(vector_of_output_variables[j], *map_output_variable_to_value[vector_of_output_variables[j]]);
			}
		}
		//a->report(std::cout);

		// increment all output variables
		int i=vector_of_output_variables.size()-1;
		while (i>=0)
		{
			//std::cout << "increment " << vector_of_hidden_variables[i]->get_name() << "\n";
			map_output_variable_to_value[vector_of_output_variables[i]]++;
			
			if (map_output_variable_to_value[vector_of_output_variables[i]] == my_values.get_vector_of_values().end())
			{
				map_output_variable_to_value[vector_of_output_variables[i]] = my_values.get_vector_of_values().begin();
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



collection_of_actions::~collection_of_actions()
{
	for (std::list<action*>::iterator i(list_of_actions.begin()); i!=list_of_actions.end(); i++)
	{
		delete *i;
	}
	list_of_actions.clear();
}


action * collection_of_actions::get(const std::map<variable*, value*> & m)
{
	for (std::list<action*>::iterator i(list_of_actions.begin()); i!=list_of_actions.end(); i++)
	{
		if ((*i)->get_match(m))
			return *i;
	}
	return NULL;

}

