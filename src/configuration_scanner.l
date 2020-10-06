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

#define YYPARSE_PARAM
 
#include "parser.tab.h"
#define YY_DECL int svarog_config_lex( YYSTYPE * yylval_param, void * yyscanner )
#define YYLTYPE int

struct class_with_config_scanner
{
	void * scanner;
	void * configuration;
};

%}

%option noyywrap 
%option reentrant
%option bison-bridge
%option yylineno
%option stack
%option prefix="svarog_config"


%x COMMENT


DIGIT    [0-9]
ID       [a-z][a-z0-9]*
BLANK           [ \t]
ANYTHING_BUT_EOL [^\n]
LETTER          [a-zA-Z_]

LETTER_OR_DIGIT ({DIGIT}|{LETTER})
IDENTIFIER		{LETTER}({LETTER_OR_DIGIT})*

%%


"#"                 yy_push_state( COMMENT, yyscanner);
     
<COMMENT>\n          yy_pop_state( yyscanner );
<COMMENT>[^\n]+      

";"						return ';';
"{"						return '{';
"}"						return '}';
"servers"				return T_SERVERS;
"server"				return T_SERVER;
"password"				return T_PASSWORD;
-?{DIGIT}+				{ yylval_param->value_int = atoi(yytext); return T_INT_LITERAL; }
\"([^\\\"]|\\.)*\"		{ yylval_param->value_string = strdup(yytext); return T_STRING_LITERAL; }

.|\n

%%


void svarog_config_open_file(struct yyguts_t* yyg, const char * f)
{
	yyin = fopen( f, "r" );
	if (yyin ==  NULL)
	{
		fprintf(stderr, "unable to open file %s\n", f);
		exit(EXIT_FAILURE);
	}
}

void svarog_config_close_file(struct yyguts_t* yyg)
{
	fclose(yyin);
}

