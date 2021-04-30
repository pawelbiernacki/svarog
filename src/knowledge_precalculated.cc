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
using namespace svarog;

knowledge_precalculated::~knowledge_precalculated()
{
	for (auto i(list_of_objects_on_visible_state.begin()); i != list_of_objects_on_visible_state.end(); ++i)
	{
		delete *i;
	}
	list_of_objects_on_visible_state.clear();
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
