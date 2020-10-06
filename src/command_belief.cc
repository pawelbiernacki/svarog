/***************************************************************************
 *   Copyright (C) 2020 by Pawel Biernacki                                 *
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


command_belief::~command_belief()
{
	for (std::list<belief_case*>::iterator i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); i++)
	{
		delete *i;
	}
}

void command_belief::execute(optimizer & o) const
{
	//std::cout << "set belief\n";
	
	if (!o.initial_input)
	{
		throw std::runtime_error("use input command to set the initial input");
	}
	
	visible_state * s = o.vs.get(*o.initial_input);
	
	if (!s)
	{
		throw std::runtime_error("failed to find the visible state for the given initial input");
	}
	
	belief * b = new belief(*s, o);
	
	for (auto i(list_of_belief_cases.begin()); i!=list_of_belief_cases.end(); i++)
	{
		b->set_probability(*(*i)->get_query(), (*i)->get_probability());
	}
	
	o.set_initial_belief(b);
}

