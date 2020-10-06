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

void statement_return::report_kuna(std::ostream & s) const
{
	s << "return ";
	inner->report_kuna(s);
	s << ";";
}


void statement_return::execute(optimizer & o, std::vector<statement*>::const_iterator & pc, const std::vector<statement*>::const_iterator & end) const
{
	/*
	std::cout << "cleaning from the stack\n";
	std::cout << "pushing value\n";
	*/
	value_or_something_else t;
	inner->get_value(o, t);
	/*
	std::cout << "got ";
	inner->report_kuna(std::cout);
	std::cout << " value ";
	t.report_kuna(std::cout);
	std::cout << "\n";
	*/
	for (int i = 0; i<stack_frame_size; i++)
	{
		o.my_stack.pop();
	}
	o.push(t, "return value");
	//o.my_stack.report_kuna(std::cout);
	pc = end;
}
