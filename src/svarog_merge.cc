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
#include "config.h"
#include "svarog.h"
#include <string.h>

int main(int argc, char * argv[])
{
	if (argc == 2)
	{
		if (!strcmp(argv[1], "-v"))
		{
			std::cout << PACKAGE_STRING << "\n";
			std::cout << "Copyright by Pawel Biernacki\n";
			std::cout << "Vantaa 2021\n";
		}
	}
	else
	if (argc != 3)
	{
		std::cout << "usage: " << argv[0] << " <file1> <file2>\n";
	}
	else
	{
			svarog::optimizer o1, o2;
			
			if (o1.parse(argv[1]))
			{
				return -1;
			}
			
			if (o2.parse(argv[2]))
			{
				return -1;
			}

			o1.merge(o2);
			
			o1.report_kuna(std::cout);
	}
	return 0;
}
