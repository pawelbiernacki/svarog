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

expression_function_call::~expression_function_call()
{
	for (std::vector<expression*>::iterator i(vector_of_arguments.begin()); i!=vector_of_arguments.end(); i++)
	{
		delete *i;
	}
	vector_of_arguments.clear();
}

void expression_function_call::add_argument(expression *e)
{
	if (e == NULL)
	{
		throw std::runtime_error("attempt to add null expression");
	}
	vector_of_arguments.push_back(e);
}


void expression_function_call::report_kuna(std::ostream & s) const
{
	bool first = true;
	s << function_name << "(";
	for (std::vector<expression*>::const_iterator i(vector_of_arguments.begin()); i!=vector_of_arguments.end(); i++)
	{
		if (!first) s << ",";
		(*i)->report_kuna(s);
		first = false;
	}
	s << ")";
}

void expression_function_call::get_value(optimizer & o, value_or_something_else & target) const
{
	function *f = o.f.get_function(function_name);
	
	if (f == NULL)
	{
		throw std::runtime_error("function not found");
	}
	
	if (vector_of_arguments.size() != f->get_vector_of_parameters().size())
	{
		throw std::runtime_error("wrong amount of arguments");		
	}
	
	
	std::vector<function::parameter*>::const_iterator j(f->get_vector_of_parameters().begin());
	
	for (std::vector<expression*>::const_iterator i(vector_of_arguments.begin()); i!=vector_of_arguments.end(); i++, j++)
	{
		value_or_something_else x;
		(*i)->get_value(o, x);
		o.push(x, (*j)->get_name());
		/*
		std::cout << "pushed ";
		(*i)->report_kuna(std::cout);
		std::cout << " result ";
		x.report_kuna(std::cout);
		std::cout << "\n";
		*/
	}
	
	f->execute(o);
	
	if (o.my_stack.size() == 0)
	{
		throw std::runtime_error("the stack is empty");
	}
	target = o.my_stack.pop();
}
