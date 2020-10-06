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
#include "parser.tab.h"
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <regex>
using namespace svarog;

#define CONFIG_PARSING_ERROR(i, x) std::cerr << "[" << __LINE__ << "] line: " << lineno() << " " << x << " expected at " << get_token_name(i) << "\n"

struct class_with_config_scanner
{
	void * scanner;
	void * configuration;
};

union lvalue {
char * value_string;
double value_float;
int value_int;
};

extern "C" int svarog_config_lex(lvalue*,void*);

extern "C" int svarog_configget_lineno(void*);


class configuration_parser
{
private:
	struct token_and_name { int token; std::string name; };
	
	static const token_and_name array_of_tokens_and_names[];

	lvalue yylval;
	
	void * scanner, * my_configuration;
	
	int lineno() { return svarog_configget_lineno(scanner); }

	int parse_servers();
		
	int parse_server();
	
	
public:
	configuration_parser(void * s, void * o) { scanner = s; my_configuration = o; }
	
	int lex();
    
	int parse();
	
	static const std::string get_token_name(int t);
};

const std::string configuration_parser::get_token_name(int t)
{
	for (int i=0; array_of_tokens_and_names[i].token; i++)
	{
		if (array_of_tokens_and_names[i].token == t)
			return array_of_tokens_and_names[i].name;
	}
	if (t == 0)
		return "EOF";
	std::stringstream s;
	s << (char)t;
	return s.str();
}


const configuration_parser::token_and_name configuration_parser::array_of_tokens_and_names[]=
{
	{ T_SERVERS, "T_SERVERS" },
	{ T_SERVER, "T_SERVER" },
	{ T_PASSWORD, "T_PASSWORD" },
	{ T_STRING_LITERAL, "T_STRING_LITERAL" },
	{ T_INT_LITERAL, "T_INT_LITERAL" },
	{ 0, "EOF" }
};

int configuration_parser::lex() 
{ 
	int i = 0;
    
	i = svarog_config_lex(&yylval, scanner); //std::cout << "token " << get_token_name(i) << "\n"; 		
	
	return i; 
}


	
int configuration_parser::parse_server()
{
	int i;
	
	i = lex();
	if (i != T_STRING_LITERAL)
	{
		CONFIG_PARSING_ERROR(i, "<string literal>");
		return -1;
	}
	
	char * new_address = yylval.value_string;
	
	i = lex();
	if (i != T_PASSWORD)
	{
		CONFIG_PARSING_ERROR(i, "password");
		free(new_address);
		return -1;		
	}

	i = lex();
	if (i != T_STRING_LITERAL)
	{
		CONFIG_PARSING_ERROR(i, "<string literal>");
		free(new_address);
		return -1;
	}
	
	
	std::string address(new_address), password(yylval.value_string);
	std::regex r("^\"([\\w\\.]+):(\\d+)\"$");
	std::smatch m,password_smatch;
	std::regex_search(address, m, r);
	
	std::regex password_regex("^\"([^\"]+)\"$");
	
	std::regex_search(password, password_smatch, password_regex);
	
	/*
	std::cout << "server " << m.str(1) << "\n";
	std::cout << "port " << m.str(2) << "\n";
	std::cout << "password " << password_smatch.str(1) << "\n";
	*/
	
	free(new_address);
	free(yylval.value_string);
	
	static_cast<configuration *>(my_configuration)->add_server(
		m.str(1), std::stoi(m.str(2)), password_smatch.str(1));
	
	i = lex();
	if (i != ';')
	{
		CONFIG_PARSING_ERROR(i, ";");
		return -1;
	}
	
	return 0;	
}


	
int configuration_parser::parse_servers()
{
	int i;
	i = lex();
	
	if (i != '{')
	{
		CONFIG_PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
	
		if (i == '}')
		{
			break;
		}
		else
		if (i == T_SERVER)
		{
			if (parse_server() == -1)
			{
				return -1;
			}
			else
				continue;
		}
		else
		{
			CONFIG_PARSING_ERROR(i, "server");
			return -1;
		}
	}
	while (i);
	return 0;
}




int configuration_parser::parse()
{
	int i;
	
	
	do
	{
		i = lex();
		
		if (!i)
			break;
		
		if (i == T_SERVERS)
		{
			if (parse_servers() == -1)
			{
				return -1;
			}
		}
		else
		{
			CONFIG_PARSING_ERROR(i, "databases or servers");
			return -1;
		}
	}
	while (true);
	
	return 0;
}

extern "C" int svarog_config_parse(class_with_config_scanner * data)
{
	configuration_parser p(data->scanner,data->configuration);
	
	return p.parse();
}
