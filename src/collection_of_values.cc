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

collection_of_values::~collection_of_values()
{
	for (std::vector<value*>::iterator i(vector_of_values.begin()); i!= vector_of_values.end(); i++)
	{
		delete *i;
	}
	vector_of_values.clear();
}

value* collection_of_values::get_value(const char * v)
{
	for (int i=0; i<vector_of_values.size(); i++)
	{
		if (vector_of_values[i]->get_name() == v)
		{
			return vector_of_values[i];
		}
	}
	return NULL;
}
