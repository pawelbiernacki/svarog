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


bool command_loop::get_is_interactive() const
{
	return true;
}

void command_loop::execute(optimizer & o) const
{
	std::cout << "loop with depth " << depth << "\n";
	
	o.list_of_input_variables.clear();
	
	for (int i=0; i<o.v.get_vector_of_variables().size(); i++)
	{
		if (o.v.get_vector_of_variables()[i]->get_is_input_variable())
		{
			o.list_of_input_variables.push_back(o.v.get_vector_of_variables()[i]);
		}
	}
	
	o.print_initial_loop_message();
	o.former_belief = NULL;
	o.current_belief = NULL;
	
	do
	{
		std::map<variable*, value*> m;
		bool eof = false;
		
		o.get_input(m, eof);
		if (eof)
			break;
		
		visible_state * s = o.vs.get(m);
		
		
		o.current_belief = new belief(*s, o);
		
		
		if (o.former_belief == NULL)
		{
			o.set_apriori_belief(o.current_belief); 	// the a-priori distribution
														// is uniform by default, but the function
														// set_apriori_belief can be
														// redefined
		}
		else 
		{
			try
			{
				o.populate_belief_for_consequence(*o.former_belief, *o.former_action, *s, *o.current_belief);
			}
			catch (std::runtime_error & e)
			{
				std::cerr << "exception caught\n";
				if (o.former_belief)
					delete o.former_belief;
				delete o.current_belief;
				throw e;
			}
		}
		
		o.print_belief(*o.current_belief);
		const action * a;
		
		if (o.my_configuration.get_has_servers())
		{
			a = o.get_optimal_action_using_servers(*o.current_belief, depth);
		}
		else
		{
			a = o.get_optimal_action(*o.current_belief, depth);
		}
			
		if (a == NULL)
		{
			std::cerr << "no optimal action found\n";
			exit(-1);
		}
		
		o.execute(a);
				
		o.former_action = a;
		if (o.former_belief)
		{
			//std::cout << "delete " << optimizer->former_belief << "\n";
			delete o.former_belief;
		}
		o.former_belief = o.current_belief;
		
		o.current_belief = NULL;		
	}
	while (true);

	delete o.former_belief;
	o.former_belief = NULL;
}
