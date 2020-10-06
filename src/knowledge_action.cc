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

knowledge_action::~knowledge_action()
{
	delete my_query;
	my_query = NULL;
	for (std::list<knowledge_case*>::iterator i(list_of_cases.begin()); i!=list_of_cases.end(); i++)
	{
		delete *i;
	}
	list_of_cases.clear();
}

void knowledge_action::add_case(knowledge_case *c)
{
	if (c == NULL)
	{
		throw std::runtime_error("empty case");
	}
	list_of_cases.push_back(c);
}


void knowledge_action::report_kuna(std::ostream & s) const
{
	s << "action ";
	my_query->report_kuna(s);
	s << "\n";
	s << ":{\n";
	for (std::list<knowledge_case*>::const_iterator i(list_of_cases.begin()); i!=list_of_cases.end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}
	s << "}\n";
}
