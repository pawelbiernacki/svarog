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
#include "config.h"
#include "svarog.h"
#include <iostream>
#include <string.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
	
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " <file>\n";
	}
	else
	{
		if (!strcmp(argv[1], "-v"))
		{
			std::cout << PACKAGE_STRING << "\n";
			std::cout << "Copyright by Pawel Biernacki\n";
			std::cout << "Vantaa 2020\n";
		}
		else
		{
			
			if ( access( svarog::optimizer::my_configuration.get_full_configuration_file_name().c_str(), F_OK ) == -1 )
			{
				svarog::optimizer::my_configuration.make_configuration_file();	// if it does not exist
			}
			
			if (svarog::optimizer::my_configuration.parse(
				svarog::optimizer::my_configuration.get_full_configuration_file_name().c_str()))
			{
				std::cerr << "configuration parsing failed\n";
			}
			
			svarog::optimizer o;
			
			if (o.parse(argv[1]))
			{
				return -1;
			}		
			o.run();
		}
	}
	return 0;
}
