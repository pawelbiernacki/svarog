/***************************************************************************
 *   Copyright (C) 2021 by Pawel Biernacki                                 *
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
#include "config.h"
using namespace svarog;


void command_cout_precalculate::dump_map_state_to_count(const visible_state & x, const std::map<state*,int> & map_state_to_count) const
{
	for (auto j(x.get_list_of_states().begin()); 
		 j!=x.get_list_of_states().end(); j++)
	{
		std::cout << "# ";
		(*j)->report_kuna(std::cout);
		std::cout << " count " << map_state_to_count.at(*j) << "\n";
	}
}

bool command_cout_precalculate::get_any_state_is_possible(const visible_state & x, optimizer & o) const
{
	for (auto j(x.get_list_of_states().begin()); 
		 j!=x.get_list_of_states().end();++j)
	{
		o.evaluating_expressions_mode = optimizer::GET_POSSIBLE;
		o.get_possible_state1 = *j;
		o.get_possible_state2 = NULL;

		if ((*j)->get_possible())
		{
			return true;
		}
	}
	return false;
}


void command_cout_precalculate::create_next_belief(belief & b, std::map<state*,int> & map_state_to_count, bool & done, bool & skip, optimizer & o) const
{
	for (auto j(b.get_visible_state().get_list_of_states().begin()); 
		 j!=b.get_visible_state().get_list_of_states().end();)
	{
		o.evaluating_expressions_mode = optimizer::GET_POSSIBLE;
		o.get_possible_state1 = *j;
		o.get_possible_state2 = NULL;

		if (!(*j)->get_possible())
		{
			map_state_to_count.at(*j) = 0;
			/*
			std::cout << "the state ";
			(*j)->report_kuna(std::cout);
			std::cout << " is impossible because "
			<< (*j)->get_why_is_impossible()
			<< " set count to 0, proceed to next state\n";
			*/
			j++;
			if (j==b.get_visible_state().get_list_of_states().end())
			{
				done = true;
			}
			
			continue;
		}
		
		
		map_state_to_count.at(*j)++;
		if (map_state_to_count.at(*j) == granularity)
		{
			map_state_to_count.at(*j) = 0;
			
			j++;
			
			if (j==b.get_visible_state().get_list_of_states().end())
			{
				done = true;
			}
			
			
			continue;
		}
		else
			break;
	}
	
	float total_count = 0.0f;
	for (auto j(b.get_visible_state().get_list_of_states().begin()); 
		 j!=b.get_visible_state().get_list_of_states().end(); j++)
	{
		total_count += map_state_to_count.at(*j);
	}
	
	if (total_count == 0.0f)
	{
		skip = true;
		return;
	}
	//std::cout << "# creating new belief for total_count " << std::showpoint << total_count << "\n";
	
	//dump_map_state_to_count(b.get_visible_state(), map_state_to_count);
	
	// normalization
	for (auto j(b.get_visible_state().get_list_of_states().begin()); 
		 j!=b.get_visible_state().get_list_of_states().end(); j++)
	{
		float p = static_cast<float>(map_state_to_count.at(*j))/total_count;
		//std::cout << "# setting " << std::showpoint << p << "\n";
		b.set_probability(*j, p);
		
	}
}

void command_cout_precalculate::execute(optimizer & o) const
{
	std::cout << "#!svarog\n";
	std::cout << "# this specification contains precalculated knowledge\n";
	std::cout << "# created by " << PACKAGE_STRING << "\n";
	std::cout << "# depth = " << depth << "\n";
	std::cout << "# granularity = " << granularity << "\n";
	
	std::cout << "values\n{\n";
	std::cout << "}\n";
	
	std::cout << "variables\n{\n";
	std::cout << "}\n";
	
	std::cout << "knowledge\n"
				<< "{\n";
	std::cout << "precalculated\n"
				<< "{\n";
	
	for (auto i(o.vs.get_list_of_visible_states().begin()); i!=o.vs.get_list_of_visible_states().end(); ++i)
	{
		if (!get_any_state_is_possible(**i, o))
		{
			continue;
		}
		
		std::cout << "on visible state (";
		(*i)->report_kuna(std::cout);
		std::cout << ")\n	{\n";
		
		// for each possible state there is <granularity> values (0..x)
		
		belief b(**i, o);
		
		std::map<state*,int> map_state_to_count;
		
		// initialize belief
		
		for (auto j((*i)->get_list_of_states().begin()); 
		 j!=(*i)->get_list_of_states().end(); j++)
		{
			b.set_probability(*j, 0.0);
			map_state_to_count.insert(std::pair<state*,int>(*j, 0));
		}		
		
		bool done = false;
		do
		{
			bool skip = false;
			create_next_belief(b, map_state_to_count, done, skip, o);
			
			if (!skip)
			{
				std::cout << "on belief (";
				b.report_kuna(std::cout);
				std::cout << ")\n		{\n";
			
				std::cout << "		action {output_action=>do_nothing};\n";
				std::cout << "		}\n";
			}
		}
		while (!done);
				
		std::cout << "	}\n";
	}
	
	std::cout << "}\n";
	std::cout << "}\n";
}
