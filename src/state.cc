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


void state::populate_map(std::map<variable*, value*> & target) const
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_hidden_variable())
		{
			target.insert(std::pair<variable*, value*>(*i, map_hidden_variable_to_value.at(*i)));
		}
		else
		if ((*i)->get_is_input_variable())
		{
			target.insert(std::pair<variable*, value*>(*i, my_visible_state.get_input_variable_value(*i)));
		}
	}
}


void state::populate_map(std::map<variable*, value*> & target)
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_hidden_variable())
		{
			target.insert(std::pair<variable*, value*>(*i, map_hidden_variable_to_value.at(*i)));
		}
		else
		if ((*i)->get_is_input_variable())
		{
			target.insert(std::pair<variable*, value*>(*i, my_visible_state.get_input_variable_value(*i)));
		}
	}
}



bool state::get_match(const std::map<variable*, value*> & m) const
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_hidden_variable())
		{
			if (map_hidden_variable_to_value.at(*i) != m.at(*i))
				return false;
		}
	}
	return true;
}


void state::report_kuna(std::ostream & s) const
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_hidden_variable())
		{
			s << (*i)->get_name() << "=>" << map_hidden_variable_to_value.at(*i)->get_name() << " ";
		}
	}
	my_visible_state.report_kuna(s);
}


bool state::get_possible() const
{	
	for (std::list<knowledge_impossible*>::const_iterator i(my_optimizer.list_of_knowledge_impossibles.begin()); i!=my_optimizer.list_of_knowledge_impossibles.end(); i++)
	{		
		if (!(*i)->get_possible(my_optimizer))
		{
			/*
			report_kuna(std::cout);
			std::cout << "\n";
			std::cout << "consider whether it is impossible for " << (*i)->get_name() << "\n";

			std::cout << "it is impossible!\n";
			*/
			return false;
		}
	}
	
	return true;
}
