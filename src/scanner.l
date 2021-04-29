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
#define YY_DECL int svarog_lex( YYSTYPE * yylval_param, void * yyscanner )
#define YYLTYPE int

struct class_with_scanner
{
	void * scanner;
	void * optimizer;
};

%}

%option noyywrap 
%option reentrant
%option bison-bridge
%option yylineno
%option stack
%option prefix="svarog"


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
","						return ',';
":"						return ':';
"{"						return '{';
"}"						return '}';
"("						return '(';
")"						return ')';
"."						return '.';
"-"                     return '-';
"?"						return '?';
"!"						return '!';
"=>"					return T_EQUAL_GREATER;
"<<"					return T_SHIFT_LEFT;
"=="                    return T_EQUAL_EQUAL;
"="						return '=';
     
"hidden"				{ return T_HIDDEN; }
"input"					{ return T_INPUT; }
"output"				{ return T_OUTPUT; }
"visible"				{ return T_VISIBLE; }
"state"					{ return T_STATE; }
"model"					{ return T_MODEL; }
"cout"					{ return T_COUT; }
"variables"				{ return T_VARIABLES; }
"variable"				{ return T_VARIABLE; }
"eol"					{ return T_EOL; }
"values"				{ return T_VALUES; }
"value"					{ return T_VALUE; }
"actions"				{ return T_ACTIONS; }
"states"				{ return T_STATES; }
"beliefs"				{ return T_BELIEFS; }
"payoff"				{ return T_PAYOFF; }
"loop"					{ return T_LOOP; }
"generator"				{ return T_GENERATOR; }
"xml"					{ return T_XML; }
"impossible"			{ return T_IMPOSSIBLE; }
"illegal"				{ return T_ILLEGAL; }
"prolog"				{ return T_PROLOG; }
"haskell"				{ return T_HASKELL; }
"assert"                { return T_ASSERT; }
"test"                  { return T_TEST; }
"class"                 { return T_CLASS; }
"objects"               { return T_OBJECTS; }
"object"                { return T_OBJECT; }
"rules"                 { return T_RULES; }
"write"					{ return T_WRITE; }
"knowledge"				{ return T_KNOWLEDGE; }
"action"				{ return T_ACTION; }
"requires"				{ return T_REQUIRES; }
"probability"			{ return T_PROBABILITY; } 
"set"					{ return T_SET; }
"case"					{ return T_CASE; }
"function"				{ return T_FUNCTION; }
"return"				{ return T_RETURN; }
"if"					{ return T_IF; }
"not"					{ return T_NOT; }
"and"					{ return T_AND; }
"or"					{ return T_OR; }
"initial"				{ return T_INITIAL; }
"terminal"				{ return T_TERMINAL; }
"perkun"				{ return T_PERKUN; }
"belief"				{ return T_BELIEF; }
"solve"					{ return T_SOLVE; }
"result"				{ return T_RESULT; }
"precalculated"			{ return T_PRECALCULATED; }
"precalculate"			{ return T_PRECALCULATE; }
"estimate"				{ return T_ESTIMATE; }
"on"					{ return T_ON; }
"too"					{ return T_TOO; }
"complex"				{ return T_COMPLEX; }

{IDENTIFIER}  			{ yylval_param->value_string = strdup(yytext); return T_IDENTIFIER; }

-?{DIGIT}+\.{DIGIT}+e(-|\+){DIGIT}+	{ yylval_param->value_float = atof(yytext); return T_FLOAT_LITERAL; }

-?{DIGIT}+\.{DIGIT}+	{ yylval_param->value_float = atof(yytext); return T_FLOAT_LITERAL; }

-?{DIGIT}+				{ yylval_param->value_int = atoi(yytext); return T_INT_LITERAL; }

\"([^\\\"]|\\.)*\"		{ yylval_param->value_string = strdup(yytext); return T_STRING_LITERAL; }

.|\n

%%


void kuna_open_file(struct yyguts_t* yyg, const char * f)
{
	yyin = fopen( f, "r" );
	if (yyin ==  NULL)
	{
		fprintf(stderr, "unable to open file %s\n", f);
		exit(EXIT_FAILURE);
	}
}

void kuna_close_file(struct yyguts_t* yyg)
{
	fclose(yyin);
}


void yyexterror (struct class_with_scanner * data, const char * m)
{
	struct yyguts_t * yyg = data->scanner;
	printf("line %u: at %s, syntax error %s\n", yylineno, yytext, m);
	//printf("syntax error %s\n", m);
}
