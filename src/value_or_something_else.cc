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


void value_or_something_else::report_kuna(std::ostream & s) const
{
	switch (mode)
	{
		case NONE: s << "NONE";
			break;
			
		case STRING_LITERAL: s << "STRING_LITERAL " << string_literal << " AS " << name;
			break;
			
		case VALUE: s << "VALUE ";
			if (my_value) s << my_value->get_name(); else s << "NULL";
			
			s << " AS " << name;
			break;
			
		case VARIABLE: s << "VARIABLE ";
			if (my_variable) s << my_variable->get_name(); else s << "NULL";

			s << " AS " << name;
			break;
	}
}
