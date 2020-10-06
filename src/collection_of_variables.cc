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


collection_of_variables::~collection_of_variables()
{
	for (std::vector<variable*>::iterator i(vector_of_variables.begin()); i!= vector_of_variables.end(); i++)
	{
		delete *i;
	}
	vector_of_variables.clear();
}

variable * collection_of_variables::get_variable(const char * v)
{
	for (int i=0; i<vector_of_variables.size(); i++)
	{
		if (vector_of_variables[i]->get_name() == v)
		{
			return vector_of_variables[i];
		}
	}
	return NULL;
}

const variable * collection_of_variables::get_variable(const char * v) const
{
	for (int i=0; i<vector_of_variables.size(); i++)
	{
		if (vector_of_variables[i]->get_name() == v)
		{
			return vector_of_variables[i];
		}
	}
	return NULL;
}

