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


void statement_if::add_statement(statement * s) 
{ 
	if (s == NULL) throw std::runtime_error("attempt to add null statement");
	vector_of_statements.push_back(s); 
}

statement_if::~statement_if()
{
	delete my_logical_expression;
	my_logical_expression = NULL;
	
	for (std::vector<statement*>::iterator i(vector_of_statements.begin()); i!=vector_of_statements.end(); i++)
	{
		delete *i;
	}
	vector_of_statements.clear();
}

void statement_if::report_kuna(std::ostream & s) const
{
	s << "if (";
	if (my_logical_expression) my_logical_expression->report_kuna(s);
	s << "){";
	
	for (std::vector<statement*>::const_iterator i(vector_of_statements.begin()); i!=vector_of_statements.end(); i++)
	{
		(*i)->report_kuna(s);
	}
	s << "}";
}

void statement_if::set_stack_frame_size(int s)
{
	for (std::vector<statement*>::iterator i(vector_of_statements.begin()); i!=vector_of_statements.end(); i++)
	{
		(*i)->set_stack_frame_size(s);
	}
}

void statement_if::execute(optimizer & o, std::vector<statement*>::const_iterator & pc, const std::vector<statement*>::const_iterator & end) const
{
	if (my_logical_expression->get_value(o))
	{
		for (std::vector<statement*>::const_iterator i(vector_of_statements.begin()); i!=vector_of_statements.end();)
		{
			(*i)->execute(o, i, vector_of_statements.end());
			if (i == vector_of_statements.end())
			{
				pc = end;
			}
			else
			{
				i++;
			}
		}
	}
}
