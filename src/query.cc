/***************************************************************************
 *   Copyright (C) 2021 by Pawel Biernacki                                 *
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


query::query(std::vector<variable*>& v, std::vector<value*>& w): 
	vector_of_variables(v), vector_of_values(w) 
{}


void query::assert_query_fact(variable* v, value * w)
{
	list_of_query_variables.push_back(v);
    insert(std::pair<variable*, value*>(v, w));
}

void query::report_kuna(std::ostream & s) const
{
	s << "{";
	bool first = true;
	for (std::list<variable*>::const_iterator i(list_of_query_variables.begin()); i!=list_of_query_variables.end(); i++)
	{
		if (!first) s << ",";
		s << (*i)->get_name() << "=>" << at(*i)->get_name() << " ";
		first = false;
	}
	s << "}";
}

query * query::get_copy(optimizer & target) const
{
	query * result = new query(target.v.get_vector_of_variables(), target.w.get_vector_of_values());
		
	for (auto i(list_of_query_variables.begin()); i!=list_of_query_variables.end(); i++)
	{
		// translate variable and value to target
		auto j(target.v.get_vector_of_variables().begin());
		while (j!=target.v.get_vector_of_variables().end() && (*j)->get_name()!=(*i)->get_name()) j++;
		
		auto k(target.w.get_vector_of_values().begin());
		while (k!=target.w.get_vector_of_values().end() && (*k)->get_name()!=at(*i)->get_name()) k++;
		
		if (j==target.v.get_vector_of_variables().end() || k==target.w.get_vector_of_values().end())
		{
			throw std::runtime_error("failed to translate query");
		}
		
		result->assert_query_fact(*j, *k);
	}
	return result;
}


bool query::get_equal(const query & q) const
{
	for (auto i(list_of_query_variables.begin()); i!=list_of_query_variables.end(); ++i)
	{
		for (auto j(q.list_of_query_variables.begin()); j!=q.list_of_query_variables.end(); ++j)
		{
			if ((*i)->get_name() == (*j)->get_name())
			{
				if (at(*i)->get_name() != q.at(*j)->get_name())
				{
					return false;
				}
			}
		}
	}
	
	return true;
}
