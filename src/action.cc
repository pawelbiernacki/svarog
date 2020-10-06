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


void action::report_kuna(std::ostream & s) const
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_output_variable())
		{
			s << (*i)->get_name() << "=" << map_output_variable_to_value.at(*i)->get_name() << " ";
		}
	}
}


bool action::get_match(const std::map<variable*, value*> & m) const
{
	for (std::vector<variable*>::const_iterator i(my_variables.get_vector_of_variables().begin());
		 i != my_variables.get_vector_of_variables().end(); i++)
	{
		if ((*i)->get_is_output_variable())
		{
			if (map_output_variable_to_value.at(*i) != m.at(*i))
				return false;
		}
	}
	return true;
}
