%{
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

struct class_with_scanner
{
	void * scanner;
	void * optimizer;
};

#define YYPARSE_PARAM data
#define YYLEX_PARAM ((struct class_with_scanner*)data)->scanner

#define PERKUN_OPTIMIZER ((struct class_with_scanner*)data)->optimizer

#define YYERROR_VERBOSE 1


// this is a cool trick to pass the parser environment to the error message function
#define yyerror(MSG) yyexterror(data, MSG)


#include <stdio.h>

%}

%union {
char * value_string;
double value_float;
int value_int;
}

%token T_INPUT T_VARIABLE T_HIDDEN T_OUTPUT T_VALUE T_VISIBLE T_STATE
%token T_MODEL T_EQUAL_GREATER T_SHIFT_LEFT
%token T_COUT T_VARIABLES T_EOL T_VALUES T_ACTIONS T_STATES T_BELIEFS
%token T_PAYOFF T_SET T_LOOP T_GENERATOR T_XML T_IMPOSSIBLE T_ILLEGAL
%token T_PROLOG T_HASKELL T_TEST T_CLASS T_OBJECT T_ASSERT T_EQUAL_EQUAL T_OBJECTS
%token T_RULES T_WRITE T_KNOWLEDGE T_ACTION T_REQUIRES T_PROBABILITY T_CASE T_RESULT
%token T_FUNCTION T_RETURN T_IF T_NOT T_AND T_OR T_INITIAL T_TERMINAL T_PERKUN T_BELIEF T_SOLVE

%token T_SERVERS T_SERVER T_PASSWORD 

%token<value_string> T_IDENTIFIER T_STRING_LITERAL
%token<value_float> T_FLOAT_LITERAL
%token<value_int> T_INT_LITERAL

%define api.pure full


%start program


%%

program:

%%

int yymyparse(void * d)
{
	return yyparse(d);
}
