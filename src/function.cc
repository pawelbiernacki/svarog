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

void function::add_parameter(parameter * p) 
{ 
	if (p == NULL) throw std::runtime_error("attempt to add null parameter");
	vector_of_parameters.push_back(p); 
}

void function::add_statement(statement * s) 
{ 
	if (s == NULL) throw std::runtime_error("attempt to add null statement");
	vector_of_statements.push_back(s); 
}

function::~function()
{
	for (std::vector<statement*>::iterator i(vector_of_statements.begin()); i!=vector_of_statements.end(); i++)
	{
		delete *i;
	}
	vector_of_statements.clear();
	
	for (std::vector<parameter*>::iterator i(vector_of_parameters.begin()); i!=vector_of_parameters.end(); i++)
	{
		delete *i;
	}
	vector_of_parameters.clear();
}

void function::report_kuna(std::ostream & s) const
{
	bool first = true;
	
	s << "function " << name << "(";
	for (std::vector<parameter*>::const_iterator i(vector_of_parameters.begin()); i!=vector_of_parameters.end(); i++)
	{
		if (!first) s << ",";
		(*i)->report_kuna(s);
		first = false;
	}
	
	s << "){";
	for (std::vector<statement*>::const_iterator i(vector_of_statements.begin()); i!=vector_of_statements.end(); i++)
	{
		(*i)->report_kuna(s);
	}
	s << "}";
}


void function::execute(optimizer & o)
{
	int i=o.my_stack.size();
	/*
	std::cout << "executing " << name << " with stack\n";
	o.my_stack.report_kuna(std::cout);
	*/
	for (std::vector<statement*>::const_iterator i(vector_of_statements.begin()); i!=vector_of_statements.end();)
	{
		/*
		std::cout << "executing ";
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
		*/
		(*i)->execute(o, i, vector_of_statements.end());
		
		if (i!=vector_of_statements.end()) i++;
	}
}

