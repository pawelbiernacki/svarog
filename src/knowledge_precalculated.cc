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
#include <cassert>
#include <iomanip>
using namespace svarog;

knowledge_precalculated::~knowledge_precalculated()
{
	for (auto i(list_of_objects_on_visible_state.begin()); i != list_of_objects_on_visible_state.end(); ++i)
	{
		delete *i;
	}
	list_of_objects_on_visible_state.clear();
}

const knowledge_precalculated::on_visible_state* knowledge_precalculated::get_object_on_visible_state(const visible_state & x) const
{
	auto i = map_visible_state_to_object_on_visible_state.find(&x);
	if (i != map_visible_state_to_object_on_visible_state.end())
	{
		return i->second;
	}
	return nullptr;
}


void knowledge_precalculated::learn(optimizer & o)
{
	for (auto i(o.vs.get_list_of_visible_states().begin()); i!=o.vs.get_list_of_visible_states().end(); ++i)
	{
		for (auto j(list_of_objects_on_visible_state.begin()); j!=list_of_objects_on_visible_state.end(); ++j)
		{
			const query * q = (*j)->get_query();
			if ((*i)->get_match(*q))
			{
				map_visible_state_to_object_on_visible_state.insert(std::pair<const visible_state*, on_visible_state*>(*i, *j));
				
				auto& mylist_on_belief((*j)->get_list_of_objects_on_belief());
				
				for (auto k(mylist_on_belief.begin()); k!=mylist_on_belief.end(); ++k)
				{
					(*k)->set_action(o.a.get(*(*k)->get_action_query()));
				}
			}
		}
	}
}


void knowledge_precalculated::merge(const knowledge_precalculated & k, optimizer & target)
{
	assert(k.get_granularity() == get_granularity());
	assert(k.get_depth() == get_depth());
	for (auto i(k.list_of_objects_on_visible_state.begin()); i!=k.list_of_objects_on_visible_state.end(); ++i)
	{
		bool found = false;
		for (auto j(list_of_objects_on_visible_state.begin()); j!=list_of_objects_on_visible_state.end(); ++j)
		{
			if ((*i)->get_query()->get_equal(*(*j)->get_query()))
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			on_visible_state * n = (*i)->get_copy(target);
			
			std::cerr << "# add on_visible_state ";
			n->get_query()->report_kuna(std::cerr);
			std::cerr << "\n";
			
			add_on_visible_state(n);
		}
	}
}


void knowledge_precalculated::report_kuna(std::ostream & s) const
{
	s << "precalculated (" << get_depth() << "," << get_granularity() << ")\n"
	<< "{\n";
	
	for (auto i(list_of_objects_on_visible_state.begin()); i != list_of_objects_on_visible_state.end(); ++i)
	{
		(*i)->report_kuna(s);
	}
	
	s << "}\n";
}

knowledge_precalculated::on_visible_state* knowledge_precalculated::on_visible_state::get_copy(optimizer & target) const
{
	on_visible_state * result = new on_visible_state(my_query->get_copy(target));
	
	result->set_amount_of_possible_states(amount_of_possible_states);
	result->set_max_amount_of_beliefs(max_amount_of_beliefs);
	result->set_too_complex(too_complex);
	
	for (auto i(list_of_objects_on_belief.begin()); i!=list_of_objects_on_belief.end(); ++i)
	{
		on_belief * ob = (*i)->get_copy(target);
		result->add_on_belief(ob);
	}
	return result;
}


void knowledge_precalculated::on_visible_state::report_kuna(std::ostream & s) const
{
	s << "on visible state (";
	my_query->report_kuna(s);
	s << ")\n";
	s << "	{\n";
	
	s << "amount_of_possible_states = " << amount_of_possible_states << ";\n";
	s << "max_amount_of_beliefs = " << max_amount_of_beliefs << ";\n";
	if (too_complex)
	{
		s << "too complex;\n";
	}
	
	for (auto i(list_of_objects_on_belief.begin()); i!=list_of_objects_on_belief.end(); ++i)
	{
		(*i)->report_kuna(s);
	}
	
	s << "	}\n";
}

knowledge_precalculated::on_visible_state::~on_visible_state()
{
	for (auto i(list_of_objects_on_belief.begin()); i!=list_of_objects_on_belief.end(); ++i)
	{
		delete *i;
	}
	list_of_objects_on_belief.clear();
	delete my_query;
}

knowledge_precalculated::on_visible_state::on_belief::~on_belief()
{
	for (auto i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); ++i)
	{
		delete *i;
	}
	list_of_belief_cases.clear();
	delete my_action_query;
}

void knowledge_precalculated::on_visible_state::on_belief::report_kuna(std::ostream & s) const
{
	s << "on belief ({\n";
	for (auto i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); ++i)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}
	s << "}\n)\n";
	s << "		{\n";
	s << "		action ";
	my_action_query->report_kuna(s);
	s << " : " << std::dec << std::showpoint << std::setw(10)
		<< action_value;
	s << ";\n";
	
	s << "		}\n";
}


float knowledge_precalculated::on_visible_state::on_belief::get_distance(const belief & b) const
{
	float result = 0.0f;
	for (auto i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); ++i)
	{
		const query * q = (*i)->get_query();
		const state * s = b.get_visible_state().get(*q);

		float delta = (*i)->get_probability()-b.get_probability(const_cast<state*>(s));
		
		result += delta*delta;
	}
	return result;
}


knowledge_precalculated::on_visible_state::on_belief* knowledge_precalculated::on_visible_state::on_belief::get_copy(optimizer & target) const
{
	on_belief * result = new on_belief();
	
	result->set_action_query(my_action_query->get_copy(target));
	
	for (auto i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); ++i)
	{
		result->add_belief_case((*i)->get_copy(target));
	}
	return result;
}

float knowledge_precalculated::on_visible_state::on_belief::get_score_for_base_belief_and_belief(const belief & b) const
{
	float result = 0.0f;
	
	for (auto i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); ++i)
	{
		// we assume this on_belief represents a base belief
		// i.e. it equals 0.0 for all states but one, for which it equals 1.0
		
		result += (*i)->get_probability()*b.get_probability(*(*i)->get_query());
	}
	
	return result;
}


void knowledge_precalculated::on_visible_state::on_belief::belief_case::report_kuna(std::ostream & s) const
{
	s << "case ";
	my_query->report_kuna(s);
	s << ":";
	s << probability << ";";
}

knowledge_precalculated::on_visible_state::on_belief::belief_case * knowledge_precalculated::on_visible_state::on_belief::belief_case::get_copy(optimizer & target) const
{
	belief_case * result = new belief_case(probability, my_query->get_copy(target));
	return result;
}
