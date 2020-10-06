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

void command_cout_states::execute(optimizer & o) const
{
	for (std::list<visible_state*>::const_iterator i(o.vs.get_list_of_visible_states().begin()); i!=o.vs.get_list_of_visible_states().end(); i++)
	{
		std::cout << "VISIBLE STATE ";
		(*i)->report_kuna(std::cout);
		std::cout << "\n";
		
		std::list<state*> & l((*i)->get_list_of_states());
		for (std::list<state*>::const_iterator j(l.begin()); j!=l.end(); j++)
		{
			(*j)->report_kuna(std::cout);
			std::cout << "\n";
		}
	}
}
