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

void belief::report_kuna(std::ostream & s) const
{
	s << "{\n";
	for (std::list<state*>::const_iterator i(my_visible_state.get_list_of_states().begin()); 
		 i!=my_visible_state.get_list_of_states().end(); i++)
	{
		if (map_state_to_probability.at(*i) > 0.0f)
		{
			s << "case ";
			(*i)->report_kuna(s);
			s << ":";
			s << std::showpoint << map_state_to_probability.at(*i) << ";\n";
		}
	}
	s << "}\n";
}

float belief::get_probability(state * s) const
{	
	if (map_state_to_probability.find(s) == map_state_to_probability.end())
	{
		return 0.0f;
	}
	
	return map_state_to_probability.at(s);
}


void belief::set_probability(state * s, float p) 
{ 
	if (map_state_to_probability.find(s) == map_state_to_probability.end())
	{
		map_state_to_probability.insert(std::pair<state*,float>(s, p)); 
	}
	else
	{
		map_state_to_probability.at(s) = p;
	}
}


void belief::make_uniform()
{
	float amount = my_visible_state.get_amount_of_possible_states();
	
	for (std::list<state*>::const_iterator i(my_visible_state.get_list_of_states().begin()); 
		 i!=my_visible_state.get_list_of_states().end(); i++)
	{
		
		my_optimizer.evaluating_expressions_mode = optimizer::GET_POSSIBLE;
		my_optimizer.get_possible_state1 = *i;
		my_optimizer.get_possible_state2 = NULL;

		if ((*i)->get_possible())
			map_state_to_probability.insert(std::pair<state*,float>(*i, 1.0f/amount));
		else
			map_state_to_probability.insert(std::pair<state*,float>(*i, 0.0f));			
	}
}
