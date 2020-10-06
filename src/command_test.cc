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


void command_test::execute_for(optimizer & o, const visible_state * i, const state * j) const
{
			
			for (std::list<action*>::const_iterator k(o.a.get_list_of_actions().begin()); k != o.a.get_list_of_actions().end(); k++)
			{
				
				o.get_illegal_visible_state1 = i;
				o.evaluating_expressions_mode = optimizer::GET_IS_ILLEGAL;
		
				if (i->get_is_illegal(*k))
				{
					continue;
				}

				float sum = 0.0f;
				
				for (std::list<visible_state*>::const_iterator i2(o.vs.get_list_of_visible_states().begin()); i2 != o.vs.get_list_of_visible_states().end(); i2++)
				{					
					for (std::list<state*>::const_iterator j2((*i2)->get_list_of_states().begin()); j2!=(*i2)->get_list_of_states().end(); j2++)
					{

						o.evaluating_expressions_mode = optimizer::GET_POSSIBLE;
						o.get_possible_state1 = j;
						o.get_possible_state2 = *j2;
					
						bool p = (*j2)->get_possible();

						if (!p) continue;
						
						o.evaluating_expressions_mode = optimizer::GET_MODEL;
						o.get_model_state1 = j;
						o.get_model_state2 = *j2;
						
						sum += o.get_model_probability(j, **k, *j2);
					}
				}
				
				if (sum < 0.99f || sum > 1.01f)
				{
					std::cout << std::showpoint << sum << " probability for ";
					j->report_kuna(std::cout);
					std::cout << " and action ";
					(*k)->report_kuna(std::cout);
					
					o.evaluating_expressions_mode = optimizer::GET_POSSIBLE;
					o.get_possible_state1 = j;
					o.get_possible_state2 = NULL;
					
					bool p = j->get_possible();
					
					if (!p) 
						std::cout << " the state is IMPOSSIBLE";
										
					std::cout << "\n";
				}
			}

}

void command_test::execute(optimizer & o) const
{
	for (std::list<visible_state*>::const_iterator i(o.vs.get_list_of_visible_states().begin());
		 i != o.vs.get_list_of_visible_states().end(); i++)
	{
		for (std::list<state*>::const_iterator j((*i)->get_list_of_states().begin()); j!=(*i)->get_list_of_states().end(); j++)
		{
			execute_for(o, *i, *j);			
		}
	}
}
