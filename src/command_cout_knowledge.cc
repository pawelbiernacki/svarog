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

void command_cout_knowledge::execute(optimizer & o) const
{
	for (std::list<knowledge_action*>::const_iterator i(o.list_of_knowledge_actions.begin()); i!= o.list_of_knowledge_actions.end(); i++)
	{
		(*i)->report_kuna(std::cout);
		std::cout << "\n";			
	}
	for (std::vector<function*>::const_iterator i(o.f.get_vector_of_functions().begin()); i!=o.f.get_vector_of_functions().end(); i++)
	{
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
	}
	
	for (std::list<knowledge_impossible*>::const_iterator i(o.list_of_knowledge_impossibles.begin()); i!=o.list_of_knowledge_impossibles.end(); i++)
	{
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
	}
	
	for (std::list<knowledge_payoff*>::const_iterator i(o.list_of_knowledge_payoffs.begin()); i!=o.list_of_knowledge_payoffs.end(); i++)
	{
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
	}
	
}
