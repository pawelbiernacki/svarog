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


clause_requires::~clause_requires()
{
	delete inner;
}
		
void clause_requires::report_kuna(std::ostream & s) const
{
	s << "requires ";
	if (inner)
	{
		inner->report_kuna(s);
	}
	s << ";";
}
		
bool clause_requires::get_match(optimizer & o) const
{	
	/*
	std::cout << "clause_requires - evaluate begin ";
	inner->report_kuna(std::cout);
	std::cout << "\n";
	*/
	bool t = inner->get_value(o);
	
	/*
	std::cout << "clause_requires - evaluate end ";
	inner->report_kuna(std::cout);
	std::cout << " returned " << (t ? "true" : "false") << "\n\n";
	*/
	return t;
}
