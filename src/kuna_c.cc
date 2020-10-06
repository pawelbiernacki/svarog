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

extern "C" void kuna_declare_list_of_values(optimizer * o)
{
	//std::cout << "declare values\n";
	o->declare_values();
}

extern "C" void kuna_declare_variables_list_of_values(optimizer * o)
{
	//std::cout << "declare variables values\n";
	o->declare_variables_values();
}

extern "C" int kuna_add_value(optimizer * o, const char * v)
{
	//std::cout << "add value " << v << "\n";
	try {
		o->add_value(v);
	}
	catch (std::runtime_error & re)
	{
		std::cerr << re.what() << "\n";
		return -1;
	}
	return 0;
}

extern "C" int kuna_create_input_variable(optimizer * o, const char * v)
{
	try {
		o->create_input_variable(v);
	}
	catch (std::runtime_error & re)
	{
		std::cerr << re.what() << "\n";
		return -1;
	}
	return 0;	
}

extern "C" int kuna_create_hidden_variable(optimizer * o, const char * v)
{
	try {
		o->create_hidden_variable(v);
	}
	catch (std::runtime_error & re)
	{
		std::cerr << re.what() << "\n";
		return -1;
	}
	return 0;	
}

extern "C" int kuna_create_output_variable(optimizer * o, const char * v)
{
	try {
		o->create_output_variable(v);
	}
	catch (std::runtime_error & re)
	{
		std::cerr << re.what() << "\n";
		return -1;
	}
	return 0;	
}
