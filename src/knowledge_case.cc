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
#include <stdexcept>
using namespace svarog;

knowledge_action::knowledge_case::~knowledge_case()
{
	for (std::list<knowledge_probability_case*>::iterator i(list_of_probability_cases.begin()); i!= list_of_probability_cases.end(); i++)
	{
		delete *i;
	}
	list_of_probability_cases.clear();
	
	for (std::list<clause*>::iterator i(list_of_clauses.begin()); i!= list_of_clauses.end(); i++)
	{
		delete *i;
	}
	list_of_clauses.clear();
}

void knowledge_action::knowledge_case::report_kuna(std::ostream & s) const
{
	s << "case " << case_name << "{\n";
	for (std::list<clause*>::const_iterator i(list_of_clauses.begin()); i!= list_of_clauses.end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}
	for (std::list<knowledge_probability_case*>::const_iterator i(list_of_probability_cases.begin()); i!= list_of_probability_cases.end(); i++)
	{
		(*i)->report_kuna(s);
		s << "\n";
	}
	s << "}\n";
}

void knowledge_action::knowledge_case::add_clause(clause * c)
{
	if (c == NULL)
	{
		throw std::runtime_error("empty clause");
	}
	list_of_clauses.push_back(c);
}

void knowledge_action::knowledge_case::add_probability_case(knowledge_probability_case *c)
{
	if (c == NULL)
	{
		throw std::runtime_error("empty probability case");
	}
	list_of_probability_cases.push_back(c);
}

bool knowledge_action::knowledge_case::get_contains_illegal() const
{
	for (std::list<clause*>::const_iterator i(list_of_clauses.begin()); i!=list_of_clauses.end(); i++)
	{
		if ((*i)->get_is_illegal())
		{
			return true;
		}
	}
	return false;
}



