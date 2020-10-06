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

bool logical_expression_equality::get_value(optimizer & o) const
{
	value_or_something_else left_value, right_value;
	
	left->get_value(o, left_value);
	right->get_value(o, right_value);
	/*
	std::cout << "logical_expression_equality check equality ";
	left->report_kuna(std::cout);
	std::cout << " and ";
	right->report_kuna(std::cout);
	std::cout << "\n";
	std::cout << "logical_expression_equality compare ";
	left_value.report_kuna(std::cout);
	std::cout << " and ";
	right_value.report_kuna(std::cout);
	std::cout << "\n";
	*/
	if (left_value.get_mode() == right_value.get_mode())
	{
		switch (left_value.get_mode())
		{
			case value_or_something_else::NONE: return false;
			case value_or_something_else::STRING_LITERAL: return left_value.get_string_literal()==right_value.get_string_literal();
			
			case value_or_something_else::VALUE: return left_value.get_value() == right_value.get_value();
			
			case value_or_something_else::VARIABLE: return left_value.get_variable() == right_value.get_variable();
		}
	}
	else
	{
		return false;
	}
	
	return false;
}
