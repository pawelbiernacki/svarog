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
#include <sstream>
using namespace svarog;

void expression_value::get_value(optimizer & o, value_or_something_else & target) const
{	
	value * w = o.get_value(value_name.c_str());
	
	if (w == NULL)
	{
		variable * v = o.get_variable(value_name.c_str());
		if (v == NULL)
		{
			std::stringstream s;
			s << "value or variable " << value_name << " not found";
			throw std::runtime_error(s.str());
		}
		else
		{
			target.set_mode(value_or_something_else::VARIABLE);	
			target.set_variable(v);
		}
	}
	else
	{
		target.set_mode(value_or_something_else::VALUE);	
		target.set_value(w);
	}
}
