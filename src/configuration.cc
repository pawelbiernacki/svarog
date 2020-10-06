/***************************************************************************
 *   Copyright (C) 2020 by Pawel Biernacki                                 *
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
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>

using namespace svarog;

const std::string configuration::get_configuration_file_name() const
{
	return "svarog.ini";
}

const std::string configuration::get_full_configuration_file_name() const
{
	std::stringstream s;
	s << "/home/" << getenv("USER") << "/.svarog" << "/" << get_configuration_file_name();
	return s.str();
}


void configuration::make_configuration_file()
{
	std::stringstream s;
	s << "/home/" << getenv("USER") << "/.svarog";
    
	if (mkdir(s.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
	{
		// config directory created
		s << "/" << get_configuration_file_name();
		std::ofstream config(s.str().c_str(), std::ios::out);
		
		if (config.good())
		{
			config 
			<< "#\n"
			<< "# svarog configuration file\n"
			<< "# copyright by Pawel Biernacki\n"
			<< "# Helsinki-Vantaa 2019-2020\n"
			<< "#\n"
			<< "#\n"
			<< "# uncomment the servers section for the servers support\n"
			<< "#servers\n"
			<< "#{\n"
			<< "#	server \"localhost:12345\" password \"12345\";\n"
			<< "#}\n";
		}
		else
		{
			std::cout << "failed to create the config file " << s.str() << "\n";
		}
	}
	else
	{
		switch (errno)
		{
			case EEXIST:	// fine - it exists already
				break;

			default:
				std::cerr << "failed to create the config dir ~/.svarog\n";
				return;	
		}
	}
}

void configuration::add_server(const std::string & a, int port, const std::string & p)
{
	configuration_server s{a, p, port};
	vector_of_configuration_servers.push_back(s);
}

int configuration::configuration_server::get_port() const
{
	return port;
}



extern "C" void svarog_config_open_file(void * scanner, const char * f);
extern "C" void svarog_config_close_file(void * scanner);
extern "C" void svarog_configlex_init(void **);
extern "C" void svarog_configlex_destroy(void *);
extern "C" int svarog_config_parse(void *);

struct class_with_config_scanner
{
	void * scanner;
	void * configuration;
};

int configuration::parse(const char * filename)
{
	class_with_config_scanner c;
	c.configuration = this;

	svarog_configlex_init ( &c.scanner );
	svarog_config_open_file(c.scanner, filename);

	int i = svarog_config_parse(&c);

	if (i != 0)
		std::cout << "error" << "\n";
	
	svarog_config_close_file(c.scanner);
	svarog_configlex_destroy (c.scanner);

	return i;
}

