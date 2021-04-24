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
#include "parser.tab.h"
#include <sstream>
#include <stdlib.h>
#include <iostream>
using namespace svarog;

#define PARSING_ERROR(i, x) *my_output_stream << "[" << __LINE__ << "] line: " << lineno() << " " << x << " expected at " << get_token_name(i) << "\n"

struct class_with_scanner
{
	void * scanner;
	void * optimizer;
};

union lvalue {
char * value_string;
double value_float;
int value_int;
};

extern "C" int svarog_lex(lvalue*,void*);

extern "C" int svarogget_lineno(void*);

extern "C" void kuna_declare_list_of_values(optimizer * o);

extern "C" void kuna_declare_variables_list_of_values(optimizer * o);

extern "C" int kuna_add_value(optimizer * o, const char * v);

extern "C" int kuna_create_input_variable(optimizer * o, const char * v);

extern "C" int kuna_create_hidden_variable(optimizer * o, const char * v);

extern "C" int kuna_create_output_variable(optimizer * o, const char * v);


class cpp_parser
{
private:
	struct token_and_name { int token; std::string name; };
	
	static const token_and_name array_of_tokens_and_names[];

	lvalue yylval;
	
	void * scanner, * kuna_optimizer;
	
	bool unlex_done, continue_flag;
	
	int next_token;

	int lineno() { return svarogget_lineno(scanner); }
	
	int parse_values();
	
	int parse_variables();
	
	int parse_knowledge();
	
	int parse_knowledge_action(knowledge_action *& target);
	
	int parse_knowledge_function(function *& target);
	
	int parse_knowledge_impossible(knowledge_impossible *& target);
	
	int parse_knowledge_payoff(knowledge_payoff *& target);
	
	int parse_logical_expression(logical_expression *& target);
	
	int parse_simple_logical_expression(logical_expression *& target);
	
	int parse_or_logical_expression(logical_expression *& target);
	
	int parse_and_logical_expression(logical_expression *& target);
	
	int parse_list_of_arguments(function & f);
	
	int parse_list_of_expressions(expression_function_call & target);
	
	int parse_expression(expression *& target);
	
	int parse_statement(statement *& target);
	
	int parse_query(query & q);
	
	int parse_knowledge_case(knowledge_action::knowledge_case *& target);
	
	int parse_knowledge_probability(knowledge_action::knowledge_case::knowledge_probability_case *& target);
	
	int parse_requires(clause_requires *& target);
		
	int parse_illegal(clause_illegal *& target);
	
	int parse_commands();
	
	int parse_belief_case(command_belief::belief_case *& target);
	
	std::ostream *my_output_stream;
	
public:
	
	cpp_parser(void * s, void * o, std::ostream * os): unlex_done(false), my_output_stream{os}
	{ scanner = s; kuna_optimizer = o; }
	
	int lex();
    
    void unlex(int);
	
	static const std::string get_token_name(int t);
	
	int parse();

};

const std::string cpp_parser::get_token_name(int t)
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

const cpp_parser::token_and_name cpp_parser::array_of_tokens_and_names[] =
{
	{ T_INPUT, "T_INPUT" },
	{ T_VARIABLE, "T_VARIABLE" },
	{ T_HIDDEN, "T_HIDDEN" },
	{ T_OUTPUT, "T_OUTPUT" },
    { T_VALUE, "T_VALUE" },
    { T_VISIBLE, "T_VISIBLE" },
    { T_STATE, "T_STATE" },
    { T_MODEL, "T_MODEL" },
    { T_EQUAL_GREATER, "T_EQUAL_GREATER" },
    { T_SHIFT_LEFT, "T_SHIFT_LEFT" },
    { T_COUT, "T_COUT" },
    { T_VARIABLES, "T_VARIABLES" },
    { T_EOL, "T_EOL" },
    { T_VALUES, "T_VALUES" },
    { T_ACTIONS, "T_ACTIONS" },
    { T_STATES, "T_STATES" },
    { T_BELIEFS, "T_BELIEFS" },
    { T_PAYOFF, "T_PAYOFF" },
    { T_SET, "T_SET" },
    { T_LOOP, "T_LOOP" },
    { T_GENERATOR, "T_GENERATOR" },
    { T_XML, "T_XML" },
    { T_IMPOSSIBLE, "T_IMPOSSIBLE" },
    { T_ILLEGAL, "T_ILLEGAL" },
    { T_PROLOG, "T_PROLOG" },
    { T_HASKELL, "T_HASKELL" },
    { T_IDENTIFIER, "T_IDENTIFIER" },
    { T_FLOAT_LITERAL, "T_FLOAT_LITERAL" },
    { T_INT_LITERAL, "T_INT_LITERAL" },
	{ T_TEST, "T_TEST" },
    { T_CLASS, "T_CLASS" },
    { T_OBJECT, "T_OBJECT" },
    { T_ASSERT, "T_ASSERT" },
    { T_OBJECTS, "T_OBJECTS" },
    { T_RULES, "T_RULES" },
	{ T_STRING_LITERAL, "T_STRING_LITERAL" },
	{ T_WRITE, "T_WRITE" },
	{ T_KNOWLEDGE, "T_KNOWLEDGE" },
	{ T_ACTION, "T_ACTION" },
	{ T_REQUIRES, "T_REQUIRES" },
	{ T_PROBABILITY, "T_PROBABILITY" },
	{ T_CASE, "T_CASE" },
	{ T_FUNCTION, "T_FUNCTION" },
	{ T_RETURN, "T_RETURN" },
	{ T_IF, "T_IF" },
	{ T_NOT, "T_NOT" },
	{ T_AND, "T_AND" },
	{ T_OR, "T_OR" },
	{ T_EQUAL_EQUAL, "==" },
	{ T_EQUAL_GREATER, "=>" },
	{ T_INITIAL, "T_INITIAL" },
	{ T_TERMINAL, "T_TERMINAL" },
	{ T_PERKUN, "T_PERKUN" },
	{ T_BELIEF, "T_BELIEF" },
	{ T_SOLVE, "T_SOLVE" },
	{ T_RESULT, "T_RESULT" },
	{ T_PRECALCULATE, "T_PRECALCULATE" },
	{ 0, "EOF" }
};



int cpp_parser::lex() 
{ 
	int i = 0;
    
    if (unlex_done)
    {
        i = next_token;
        unlex_done = false;
    }
	else
    {
        i = svarog_lex(&yylval, scanner); // std::cout << "token " << get_token_name(i) << "\n"; 		
    }
	return i; 
}


void cpp_parser::unlex(int t)
{
    if (!unlex_done)
    {
        unlex_done = true;
        next_token = t;
    }
}


int cpp_parser::parse_values()
{
	int i;
	
	i = lex();
	if (i != T_VALUES)
	{
		PARSING_ERROR(i, "values");
		return -1;
	}
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		
		if (i == '}')
			break;
		
		if (i == T_VALUE)
		{
			
			kuna_declare_list_of_values((optimizer *)kuna_optimizer);
			
			do
			{
				i = lex();
				if (i == T_IDENTIFIER)
				{
					if (kuna_add_value((optimizer *)kuna_optimizer, yylval.value_string) == -1)
					{
						PARSING_ERROR(i, "appropriate value");
						free(yylval.value_string);
						return -1;
					}
					free(yylval.value_string);
				}
				else
				{
					PARSING_ERROR(i, "identifier");
					return -1;
				}
				
				i = lex();
				if (i == ',')
					continue;
				else
				if (i == ';')
					break;
				else
				{
					PARSING_ERROR(i, ";");
					return -1;
				}
			}
			while (true);
		}
		else
		{
			PARSING_ERROR(i, "value");
			return -1;
		}
	}
	while (true);
			
	return 0;
}
	
int cpp_parser::parse_variables()
{
	int i;
	
	i = lex();
	if (i != T_VARIABLES)
	{
		PARSING_ERROR(i, "variables");
		return -1;
	}
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}

	do
	{
		i = lex();
		
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		
		if (i == '}')
			break;
		
		int mode = 0;
		
		if (i == T_INPUT)
		{
			mode = T_INPUT;
		}
		else
		if (i == T_OUTPUT)
		{
			mode = T_OUTPUT;
		}
		else
		if (i == T_HIDDEN)
		{
			mode = T_HIDDEN;
		}
		else
		{
			PARSING_ERROR(i, "input or output or hidden");
			return -1;
		}
		
		i = lex();
		
		if (i == T_VARIABLE)
		{
			do
			{
				i = lex();
				if (i == T_IDENTIFIER)
				{
					switch (mode)
					{
						case T_INPUT:
							if (kuna_create_input_variable((optimizer*)kuna_optimizer, yylval.value_string) == -1)
							{
								free(yylval.value_string);
								PARSING_ERROR(i, "variable name");
								return -1;								
							}
							break;
						case T_OUTPUT:
							if (kuna_create_output_variable((optimizer*)kuna_optimizer, yylval.value_string) == -1)
							{
								free(yylval.value_string);
								PARSING_ERROR(i, "variable name");
								return -1;								
							}
							break;
						case T_HIDDEN:
							if (kuna_create_hidden_variable((optimizer*)kuna_optimizer, yylval.value_string) == -1)
							{
								free(yylval.value_string);
								PARSING_ERROR(i, "variable name");
								return -1;								
							}
							break;
					}
					
					kuna_declare_variables_list_of_values((optimizer*)kuna_optimizer);
					free(yylval.value_string);
				}
				else
				{
					PARSING_ERROR(i, "identifier");
					return -1;
				}
				
				
				i = lex();
				
				if (i == ':')
				{
				}
				else
				{
					PARSING_ERROR(i, ":");
					return -1;
				}
				
				i = lex();
				
				if (i == '{')
				{
				}
				else
				{
					PARSING_ERROR(i, "{");
					return -1;
				}
				
				// parse list of values
				do
				{
					i = lex();
					if (i == T_IDENTIFIER)
					{
						if (kuna_add_value((optimizer*)kuna_optimizer, yylval.value_string) == -1)
						{
							PARSING_ERROR(i, "value");
							free(yylval.value_string);
							return -1;
						}
						free(yylval.value_string);
					}
					else
					{
						PARSING_ERROR(i, "identifier");
						return -1;
					}
					
					i = lex();
					if (i == ',')
						continue;
					else
					if (i == '}')
						break;
					else
					{
						PARSING_ERROR(i, "}");
						return -1;
					}
				}
				while (true);

				i = lex();
				if (i == ',')
					continue;
				else
				if (i == ';')
					break;
				else
				{
					PARSING_ERROR(i, ";");
					return -1;
				}
			}
			while (true);
		}
		else
		{
			PARSING_ERROR(i, "variable");
			return -1;
		}
	}
	while (true);

	return 0;
}

int cpp_parser::parse_query(query & q)
{
	int i;
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	i = lex();
	
	do
	{
		if (i == 0)
		{
            PARSING_ERROR(i, "}");	
			return -1;
		}
		
		if (i == '}')
			break;
        
        if (i != T_IDENTIFIER)
		{
            PARSING_ERROR(i, "identifier");	
			return -1;
		}
		
		char * variable_name = yylval.value_string;
        		
		i = lex();
		
		if (i == T_EQUAL_GREATER)
		{
		}
		else
		{
            PARSING_ERROR(i, "=>");	
			free(variable_name);
			return -1;
		}
		
		i = lex();
        if (i != T_IDENTIFIER)
		{
            PARSING_ERROR(i, "identifier");	
			free(variable_name);
			return -1;
		}
		
		char * value_name = yylval.value_string;
		
		variable * v = ((optimizer*)kuna_optimizer)->v.get_variable(variable_name);
		value * w = ((optimizer*)kuna_optimizer)->w.get_value(value_name);
		
		if (!v)
		{
			PARSING_ERROR(i, "existing variable");
			free(variable_name);
			free(value_name);
			return -1;
		}

		if (!w)
		{
			PARSING_ERROR(i, "existing value");
			free(variable_name);
			free(value_name);
			return -1;
		}

        q.assert_query_fact(v, w);
        
		i = lex();
		if (i == ',')
		{
			i = lex();
			continue;
		}
		else
		if (i == '}')
		{
			break;
		}
		else
		{
            PARSING_ERROR(i, "}");	
			return -1;
		}
	}
	while (true);
    
    /*
    std::cout << "parsed query ";
    q.report(std::cout);
    std::cout << "\n";
	*/	
	return 0;
}

int cpp_parser::parse_requires(clause_requires *& target)
{
	int i;
	
	i = lex();
	
	if (i != T_REQUIRES)
	{
		PARSING_ERROR(i, "requires");
		return -1;
	}
	
	logical_expression * e = NULL;
	if (parse_logical_expression(e))
	{
		delete e;
		return -1;
	}
	
	target = new clause_requires(e);
			
	i = lex();
	if (i != ';')
	{
		PARSING_ERROR(i, ";");
		return -1;
	}
			
	return 0;
}
	
	

int cpp_parser::parse_expression(expression *& target)
{
	int i;
	i = lex();
	
	
	if (i == T_STRING_LITERAL)
	{
		target = new expression_string_literal(yylval.value_string, lineno());
		free(yylval.value_string);
		return 0;
	}
	else
	if (i == T_IDENTIFIER)
	{
		char * value_name = yylval.value_string;
	
		i = lex();
		if (i == '(')
		{
			target = new expression_function_call(value_name, lineno());
		
			expression_function_call * f = (expression_function_call*)target;
		
			if (parse_list_of_expressions(*f))
			{
				return -1;
			}
			i = lex();
		
			if (i != ')')
			{
				PARSING_ERROR(i, ")");
				return -1;
			}
		}
		else
		{
			unlex(i);
			target = new expression_value(value_name, lineno());
			free(value_name);
		}
	}
	else
	if (i == T_INITIAL)
	{
		i = lex();
		if (i != T_VALUE)
		{
			PARSING_ERROR(i, "value");
			return -1;
		}
		i = lex();
		if (i != T_IDENTIFIER)
		{
			PARSING_ERROR(i, "identifier");
			return -1;
		}
		target = new expression_initial_variable_value(yylval.value_string, lineno());
		free(yylval.value_string);
	}
	else
	if (i == T_TERMINAL)
	{
		i = lex();
		if (i != T_VALUE)
		{
			PARSING_ERROR(i, "value");
			return -1;
		}
		i = lex();
		if (i != T_IDENTIFIER)
		{
			PARSING_ERROR(i, "identifier");
			return -1;
		}
		target = new expression_terminal_variable_value(yylval.value_string, lineno());
		free(yylval.value_string);
	}
	else
	{
		PARSING_ERROR(i, "identifier or initial or terminal");
		return -1;
	}
	return 0;
}

	

int cpp_parser::parse_knowledge_probability(knowledge_action::knowledge_case::knowledge_probability_case *& target)
{
	int i;
	
	i = lex();
	if (i != T_PROBABILITY)
	{
		PARSING_ERROR(i, "probability");
		return -1;
	}

	i = lex();
	if (i != T_STRING_LITERAL)
	{
		PARSING_ERROR(i, "string literal");
		return -1;
	}
	char * case_name = yylval.value_string;
	
	i = lex();
	if (i != ':')
	{
		PARSING_ERROR(i, ":");
		free(case_name);
		return -1;
	}
	
	i = lex();
	if (i != T_FLOAT_LITERAL)
	{
		PARSING_ERROR(i, "float literal");
		free(case_name);
		return -1;
	}
	
	if (yylval.value_float < 0.0 || yylval.value_float > 1.0)
	{
		PARSING_ERROR(i, "float literal 0.0..1.0");
		free(case_name);
		return -1;
	}
	
	target = new knowledge_action::knowledge_case::knowledge_probability_case(case_name, yylval.value_float);
	free(case_name);
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		if (i == '}')
			break;
		
		if (i == T_REQUIRES)
		{
			unlex(T_REQUIRES);
			clause_requires * x = NULL;
			if (parse_requires(x))
			{
				delete x;
				return -1;
			}
			target->add_clause(x);			
		}
		else
		{
			PARSING_ERROR(i, "requires");
			return -1;
		}
	}
	while (true);
			
			
	return 0;
}

int cpp_parser::parse_illegal(clause_illegal *& target)
{
	int i;
	i = lex();
	
	if (i != T_ILLEGAL)
	{
		PARSING_ERROR(i, "illegal");
		return -1;
	}
	i = lex();
	if (i != ';')
	{
		PARSING_ERROR(i, ";");
		return -1;
	}
	
	target = new clause_illegal();
	
	return 0;
}

	

int cpp_parser::parse_knowledge_case(knowledge_action::knowledge_case *& target)
{
	int i;
	
	i = lex();
	if (i != T_CASE)
	{
		PARSING_ERROR(i, "case");
		return -1;
	}
		
	i = lex();
	if (i != T_STRING_LITERAL)
	{
		PARSING_ERROR(i, "string literal");
		return -1;
	}
	
	target = new knowledge_action::knowledge_case(yylval.value_string);

	free(yylval.value_string);
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		if (i == '}')
			break;
		
		if (i == T_REQUIRES)
		{
			unlex(T_REQUIRES);
			
			clause_requires * r;
			if (parse_requires(r))
			{
				delete r;
				return -1;
			}
			target->add_clause(r);
		}
		else
		if (i == T_PROBABILITY)
		{
			unlex(T_PROBABILITY);
			
			knowledge_action::knowledge_case::knowledge_probability_case * c = NULL;
			if (parse_knowledge_probability(c))
			{
				delete c;
				return -1;
			}
			
			target->add_probability_case(c);
		}
		else
		if (i == T_ILLEGAL)
		{
			unlex(T_ILLEGAL);
			clause_illegal *x = NULL;
			if (parse_illegal(x))
			{
				delete x;
				return -1;
			}
			target->add_clause(x);
		}
		else
		{
			PARSING_ERROR(i, "requires or probability or illegal");
			return -1;
		}
		
	}
	while (true);
	
	
	return 0;
}


int cpp_parser::parse_knowledge_action(knowledge_action *& target)
{
	int i;
	
	i = lex();
	if (i != T_ACTION)
	{
		PARSING_ERROR(i, "action");
		return -1;
	}
	
	query *qi = new query(((optimizer*)kuna_optimizer)->v.get_vector_of_variables(), ((optimizer*)kuna_optimizer)->w.get_vector_of_values());
	if (parse_query(*qi) == -1)
	{
		delete qi;
		return -1;
	}
	
	target = new knowledge_action(qi);
	
	i = lex();
	
	if (i != ':')
	{
		PARSING_ERROR(i, ":");
		return -1;
	}
	
	i = lex();
	
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		
		if (i == '}')
			break;
		
		if (i == T_CASE)
		{
			unlex(T_CASE);
			knowledge_action::knowledge_case *c = NULL;
			if (parse_knowledge_case(c))
			{
				return -1;
			}
			target->add_case(c);
		}
	}
	while (true);
	
	return 0;
}

int cpp_parser::parse_list_of_expressions(expression_function_call & target)
{
	int i;

	do
	{
		i = lex();
		if (i == ')')
		{
			unlex(i);
			break;
		}
		unlex(i);
		
		expression * e = NULL;
		if (parse_expression(e))
		{
			delete e;
			return -1;
		}
		target.add_argument(e);
		
		i = lex();
		if (i == ',')
		{
			continue;
		}
		else
		if (i == ')')
		{
			unlex(i);
			break;
		}
		else
		{
			PARSING_ERROR(i, ", or )");
			return -1;
		}
	}
	while (true);
	
	return 0;
}

int cpp_parser::parse_simple_logical_expression(logical_expression *& target)
{
	int i;
	
	i = lex();
	
	if (i == T_NOT)
	{
		logical_expression * e = NULL;
		if (parse_simple_logical_expression(e))
		{
			delete e;
			return -1;
		}

		target = new logical_expression_not(e, lineno());
	}
	else
	if (i == '(')
	{
		logical_expression * e = NULL;
		if (parse_logical_expression(e))
		{
			delete e;
			return -1;
		}
		target = e;
		
		i = lex();
		if (i != ')')
		{
			PARSING_ERROR(i, ")");
			return -1;
		}
	}
	else
	{
		unlex(i);
		
		expression * e1 = NULL;
		if (parse_expression(e1))
		{
			delete e1;
			return -1;
		}		
		
		i = lex();
		
		if (i == T_EQUAL_EQUAL)
		{
		}
		else
		{
			PARSING_ERROR(i, "==");
			delete e1;
			return -1;
		}
		
		expression * e2=NULL;
		if (parse_expression(e2))
		{
			delete e1;
			delete e2;
			return -1;
		}

		target = new logical_expression_equality(e1, e2, lineno());
	}
	return 0;
}

int cpp_parser::parse_or_logical_expression(logical_expression *& target)
{
	int i;
	
	logical_expression * e1 = NULL;
	
	if (parse_and_logical_expression(e1))
	{
		delete e1;
		return -1;
	}
	
	i = lex();
	if (i == T_OR)
	{
		logical_expression * e2 = NULL;
		
		if (parse_or_logical_expression(e2))
		{
			delete e1;
			delete e2;
			return -1;
		}
		
		target = new logical_expression_or(e1, e2, lineno());
	}
	else
	{
		unlex(i);
		target = e1;
	}
	return 0;
}
	
int cpp_parser::parse_and_logical_expression(logical_expression *& target)
{
	int i;
	
	logical_expression * e1 = NULL;
	
	if (parse_simple_logical_expression(e1))
	{
		delete e1;
		return -1;
	}
	
	i = lex();
	if (i == T_AND)
	{
		logical_expression * e2 = NULL;
		
		if (parse_and_logical_expression(e2))
		{
			delete e1;
			delete e2;
			return -1;
		}
		target = new logical_expression_and(e1, e2, lineno());
	}
	else
	{
		unlex(i);
		target = e1;
	}
	return 0;
}


int cpp_parser::parse_logical_expression(logical_expression *& target)
{
	if (parse_or_logical_expression(target))
	{
		return -1;
	}
	return 0;
}

int cpp_parser::parse_statement(statement *& target)
{
	int i;
	
	i = lex();
	
	if (i == T_RETURN)
	{
		expression * e = NULL;
		
		if (parse_expression(e))
		{
			delete e;
			return -1;
		}
		
		target = new statement_return(e);
		
		
		i = lex();
		if (i != ';')
		{
			PARSING_ERROR(i, ";");
			return -1;
		}
	}
	else
	if (i == T_IF)
	{
		target = new statement_if();
		
		i = lex();
		if (i != '(')
		{
			PARSING_ERROR(i, "(");
			return -1;
		}
		logical_expression * e = NULL;
		
		if (parse_logical_expression(e))
		{
			delete e;
			return -1;
		}
		
		((statement_if*)target)->set_condition(e);
		
		i = lex();
		if (i != ')')
		{
			PARSING_ERROR(i, ")");
			return -1;
		}
		i = lex();
		if (i != '{')
		{
			PARSING_ERROR(i, "{");
			return -1;
		}
		do
		{
			i = lex();
			if (i == 0)
			{
				PARSING_ERROR(i, "}");
				return -1;
			}
			if (i == '}')
			{
				break;
			}
			statement * s = NULL;
			unlex(i);
			
			
			if (parse_statement(s))
			{
				delete s;
				return -1;
			}
			
			if (s == NULL)
			{
				PARSING_ERROR(i, "internal error, statement");
				return -1;
			}

			((statement_if*)target)->add_statement(s);
		}
		while (true);
	}
	else
	{
		unlex(i);
		
		expression * e = NULL;
		if (parse_expression(e))
		{
			delete e;
			return -1;
		}
		target = new statement_expression(e);
		
		i = lex();
		if (i != ';')
		{
			PARSING_ERROR(i, ";");
			return -1;
		}
	}
	
	if (target == NULL)
	{
		PARSING_ERROR(i, "internal error, statement");
		return -1;		
	}
	
	return 0;
}

int cpp_parser::parse_list_of_arguments(function & f)
{
	int i;
	int m;
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, ")");
			return -1;
		}
		
		if (i == ')')
		{
			unlex(i);
			break;
		}
		
		if (i == T_VARIABLE)
		{
			// mode variable
			m = 1;
		}
		else
		if (i == T_VALUE)
		{
			// mode value
			m = 2;
		}
		else
		{
			PARSING_ERROR(i, "variable or value");
			return -1;
		}
		
		i = lex();
		
		if (i != T_IDENTIFIER)
		{
			PARSING_ERROR(i, "identifier");
			return -1;
		}
		
		switch (m)
		{
			case 1:
				f.add_parameter(new function::parameter_variable(yylval.value_string));
				break;
				
			case 2:
				f.add_parameter(new function::parameter_value(yylval.value_string));
				break;
		}
		
		free(yylval.value_string);
		
		i = lex();
		if (i == ',')
		{
			continue;
		}
		else
		{
			unlex(i);
		}
	}
	while (true);
	
	return 0;
}


int cpp_parser::parse_knowledge_impossible(knowledge_impossible *& target)
{
	int i;
	i = lex();
	
	if (i!=T_IMPOSSIBLE)
	{
		PARSING_ERROR(i, "impossible");
		return -1;
	}
	
	i = lex();
	if (i != T_STRING_LITERAL)
	{
		PARSING_ERROR(i, "string literal");
		return -1;
	}
	
	target = new knowledge_impossible(yylval.value_string);
	
	free(yylval.value_string);
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		if (i == '}')
			break;
		
		if (i == T_REQUIRES)
		{
			unlex(T_REQUIRES);
			
			clause_requires * r;
			if (parse_requires(r))
			{
				delete r;
				return -1;
			}
			target->add_clause(r);
		}
		else
		{
			PARSING_ERROR(i, "requires");
			return -1;
		}
		
	}
	while (true);
	
	return 0;
}



int cpp_parser::parse_knowledge_function(function *& target)
{
	int i;
	
	i = lex();
	if (i != T_FUNCTION)
	{
		PARSING_ERROR(i, "function");
		return -1;
	}
	i = lex();
	if (i != T_IDENTIFIER)
	{
		PARSING_ERROR(i, "identifier");
		return -1;
	}
	
	target = new function(yylval.value_string);
	
	i = lex();
	if (i != '(')
	{
		PARSING_ERROR(i, "(");
		return -1;
	}

	if (parse_list_of_arguments(*target))
	{
		return -1;
	}

	i = lex();
	if (i != ')')
	{
		PARSING_ERROR(i, ")");
		return -1;
	}

	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		if (i == '}')
		{
			break;
		}
		unlex(i);
		
		statement * s = NULL;
		
		if (parse_statement(s))
		{
			delete s;
			return -1;
		}
		s->set_stack_frame_size(target->get_vector_of_parameters().size());
		
		if (s == NULL)
		{
			PARSING_ERROR(i, "internal error, statement");
			return -1;
		}
		target->add_statement(s);
	}
	while (true);

	
	return 0;
}


int cpp_parser::parse_knowledge_payoff(knowledge_payoff *& target)
{
	int i;
	
	i = lex();
	if (i != T_PAYOFF)
	{
		PARSING_ERROR(i, "payoff");
		return -1;
	}
	
	i = lex();
	if (i != T_STRING_LITERAL)
	{
		PARSING_ERROR(i, "string literal");
		return -1;
	}
	
	char * payoff_comment = yylval.value_string;
	
	
	i = lex();
	if (i != ':')
	{
		PARSING_ERROR(i, ":");
		free(payoff_comment);
		return -1;
	}

	i = lex();
	if (i != T_FLOAT_LITERAL)
	{
		PARSING_ERROR(i, "float literal");
		free(payoff_comment);
		return -1;
	}
	target = new knowledge_payoff(payoff_comment, yylval.value_float);

	free(payoff_comment);
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		if (i == '}')
			break;
		
		if (i == T_REQUIRES)
		{
			unlex(T_REQUIRES);
			
			clause_requires * r;
			if (parse_requires(r))
			{
				delete r;
				return -1;
			}
			target->add_clause(r);
		}
		else
		{
			PARSING_ERROR(i, "requires");
			return -1;
		}
		
	}
	while (true);
	
	
	
	return 0;
}


	
int cpp_parser::parse_knowledge()
{
	int i;
	
	i = lex();
	if (i != T_KNOWLEDGE)
	{
		PARSING_ERROR(i, "knowledge");
		return -1;
	}
	
	i = lex();
	if (i != '{')
	{
		PARSING_ERROR(i, "{");
		return -1;
	}
	
	do
	{
		i = lex();
		if (i == 0)
		{
			PARSING_ERROR(i, "}");
			return -1;
		}
		
		if (i == '}')
			break;

		if (i == T_ACTION)
		{
			unlex(T_ACTION);
			
			knowledge_action * a = NULL;
			
			if (parse_knowledge_action(a))
			{
				return -1;
			}
			((optimizer*)kuna_optimizer)->add_knowledge_action(a);
		}
		else
		if (i == T_FUNCTION)
		{
			unlex(T_FUNCTION);
			
			function * f1 = NULL;
			
			if (parse_knowledge_function(f1))
			{
				delete f1;
				return -1;
			}
			((optimizer*)kuna_optimizer)->f.add_function(f1);
		}
		else
		if (i == T_IMPOSSIBLE)
		{
			unlex(T_IMPOSSIBLE);
			
			knowledge_impossible * x = NULL;
			if (parse_knowledge_impossible(x))
			{
				delete x;
				return -1;
			}
			((optimizer*)kuna_optimizer)->add_knowledge_impossible(x);
		}
		else
		if (i == T_PAYOFF)
		{
			unlex(T_PAYOFF);
			knowledge_payoff * x = NULL;
			if (parse_knowledge_payoff(x))
			{
				delete x;
				return -1;
			}
			((optimizer*)kuna_optimizer)->add_knowledge_payoff(x);
		}
		else
		{
			PARSING_ERROR(i, "action or function or impossible or payoff");
			return -1;
		}
	}
	while (true);
	

	return 0;
}

int cpp_parser::parse_belief_case(command_belief::belief_case *& target)
{
	int i;
	
	i=lex();
	
	if (i!=T_CASE)
	{
		PARSING_ERROR(i, "case");
		return -1;
	}
	
	
	
	query *qi = new query(((optimizer*)kuna_optimizer)->v.get_vector_of_variables(), ((optimizer*)kuna_optimizer)->w.get_vector_of_values());
	if (parse_query(*qi) == -1)
	{
		delete qi;
		return -1;
	}

	i = lex();
	if (i!=':')
	{
		PARSING_ERROR(i, ":");
		delete qi;
		return -1;
	}
	
	i = lex();
	if (i!=T_FLOAT_LITERAL)
	{
		PARSING_ERROR(i, "<float literal>");
		delete qi;
		return -1;
	}

	if (yylval.value_float < 0.0 || yylval.value_float > 1.0)
	{
		PARSING_ERROR(i, "float literal 0.0..1.0");
		delete qi;
		return -1;
	}
	target = new command_belief::belief_case(qi, yylval.value_float);
	
	i = lex();
	if (i!=';')
	{
		PARSING_ERROR(i, ";");
		delete qi;
		return -1;
	}
	
	return 0;
}


int cpp_parser::parse_commands()
{
	int i;
	
	do 
	{
		i = lex();
		if (i == 0)
			break;
		
		if (i == T_LOOP)
		{
			i = lex();
			if (i != '(')
			{
				PARSING_ERROR(i, "(");
				return -1;
			}
			i = lex();
			if (i != T_INT_LITERAL)
			{
				PARSING_ERROR(i, "int literal");
				return -1;
			}
			if (yylval.value_int < 0)
			{
				PARSING_ERROR(i, "int literal >=0");
				return -1;
			}
			
			
			command_loop * c = new command_loop(yylval.value_int);
			((optimizer*)kuna_optimizer)->add_command(c);
			i = lex();
			if (i != ')')
			{
				PARSING_ERROR(i, ")");
				return -1;
			}
			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}
		}
		else
		if (i == T_INPUT)
		{
			
			i = lex();
			if (i == '{')
			{
				unlex(i);

				query *qi = new query(((optimizer*)kuna_optimizer)->v.get_vector_of_variables(), ((optimizer*)kuna_optimizer)->w.get_vector_of_values());
				if (parse_query(*qi) == -1)
				{
					delete qi;
					return -1;
				}
				command_input * c = new command_input(qi);
				((optimizer*)kuna_optimizer)->add_command(c);				
			}
			else
			{
				PARSING_ERROR(i, "{");
				return -1;
			}

			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}			
		}
		else
		if (i == T_ACTION)
		{
			i = lex();
			if (i == '{')
			{
				unlex(i);

				query *qi = new query(((optimizer*)kuna_optimizer)->v.get_vector_of_variables(), ((optimizer*)kuna_optimizer)->w.get_vector_of_values());
				if (parse_query(*qi) == -1)
				{
					delete qi;
					return -1;
				}
				command_action * c = new command_action(qi);
				((optimizer*)kuna_optimizer)->add_command(c);				
			}
			else
			{
				PARSING_ERROR(i, "{");
				return -1;
			}

			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}			
		}
		else
		if (i == T_SOLVE)
		{
			i = lex();
			if (i != '(')
			{
				PARSING_ERROR(i, "(");
				return -1;
			}
			i = lex();
			if (i != T_INT_LITERAL)
			{
				PARSING_ERROR(i, "<int literal>");
				return -1;
			}
			if (yylval.value_int < 0)
			{
				PARSING_ERROR(i, "int literal >=0");
				return -1;
			}
			
			command_solve * c = new command_solve(yylval.value_int);
			((optimizer*)kuna_optimizer)->add_command(c);				
			i = lex();
			if (i != ')')
			{
				PARSING_ERROR(i, ")");
				return -1;
			}
			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}						
		}
		else
		if (i == T_BELIEF)
		{
			i = lex();
			if (i != '{')
			{
				PARSING_ERROR(i, "{");
				return -1;
			}

			command_belief * c = new command_belief();
			((optimizer*)kuna_optimizer)->add_command(c);
			
			do
			{
				i = lex();
				if (i == '}')
				{
					break;
				}
				else
				{
					unlex(i);
					
					command_belief::belief_case * ca = NULL;
					if (parse_belief_case(ca) == -1)
					{
						delete ca;
						return -1;
					}
					c->add(ca);
				}
			}
			while (true);
			
			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}
		}
		else
		if (i == T_COUT)
		{
			i = lex();
			if (i != T_SHIFT_LEFT)
			{
				PARSING_ERROR(i, "<<");
				return -1;
			}
			i = lex();
			if (i == T_KNOWLEDGE)
			{
				command_cout_knowledge * c = new command_cout_knowledge();
				((optimizer*)kuna_optimizer)->add_command(c);
			}
			else
			if (i == T_VISIBLE)
			{
				i = lex();
				if (i == T_STATES)
				{
					command_cout_visible_states * c = new command_cout_visible_states();
					((optimizer*)kuna_optimizer)->add_command(c);
				}
				else
				{
					PARSING_ERROR(i, "states");
					return -1;
				}
			}
			else
			if (i == T_STATES)
			{
				command_cout_states * c = new command_cout_states();
				((optimizer*)kuna_optimizer)->add_command(c);
			}
			else
			if (i == T_PERKUN)
			{
				command_cout_perkun * c = new command_cout_perkun();
				((optimizer*)kuna_optimizer)->add_command(c);
			}
			else
			if (i == T_RESULT)
			{
				command_cout_result * c = new command_cout_result();
				((optimizer*)kuna_optimizer)->add_command(c);
			}
			else
			if (i == T_PRECALCULATE)
			{
				i = lex();
				if (i != '(')
				{
					PARSING_ERROR(i, "(");
					return -1;
				}
				
				i = lex();
				if (i != T_INT_LITERAL)
				{
					PARSING_ERROR(i, "int literal");
					return -1;
				}
				int depth = yylval.value_int;
				
				i = lex();
				if (i != ',')
				{
					PARSING_ERROR(i, ",");
					return -1;
				}

				i = lex();
				if (i != T_INT_LITERAL)
				{
					PARSING_ERROR(i, "int literal");
					return -1;
				}
				
				if (yylval.value_int < 2)
				{
					PARSING_ERROR(i, "granularity (second argument) should be >=2");
					return -1;
				}
				
				command_cout_precalculate * c = new command_cout_precalculate(depth, yylval.value_int);
				((optimizer*)kuna_optimizer)->add_command(c);


				i = lex();
				if (i != ')')
				{
					PARSING_ERROR(i, ")");
					return -1;
				}
				
			}
			else
			{
				PARSING_ERROR(i, "knowledge or visible or states or perkun or result");
				return -1;
			}
						
			i = lex();
			if (i != T_SHIFT_LEFT)
			{
				PARSING_ERROR(i, "<<");
				return -1;
			}
			i = lex();
			if (i != T_EOL)
			{
				PARSING_ERROR(i, "eol");
				return -1;
			}
			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}
		}
		else
		if (i == T_TEST)
		{			
			i = lex();
			if (i != '(')
			{
				PARSING_ERROR(i, "(");
				return -1;
			}
			i = lex();
			if (i == '{')
			{
				unlex(i);

				query *qi = new query(((optimizer*)kuna_optimizer)->v.get_vector_of_variables(), ((optimizer*)kuna_optimizer)->w.get_vector_of_values());
				if (parse_query(*qi) == -1)
				{
					delete qi;
					return -1;
				}
				command_test_with_query * c = new command_test_with_query(qi);
				((optimizer*)kuna_optimizer)->add_command(c);
				
				i = lex();
				if (i != ')')
				{
					PARSING_ERROR(i, ")");
					return -1;
				}				
			}
			else
			{
				command_test * c = new command_test();
				((optimizer*)kuna_optimizer)->add_command(c);
				if (i != ')')
				{
					PARSING_ERROR(i, ")");
					return -1;
				}
			}
			
			i = lex();
			if (i != ';')
			{
				PARSING_ERROR(i, ";");
				return -1;
			}
		}
		else
		{
			PARSING_ERROR(i, "loop or cout or test or input or action or belief or solve");
			return -1;
		}
	}
	while (true);
	
	return 0;
}



int cpp_parser::parse()
{
    int i;
	
	if (parse_values())
	{
		return -1;
	}
	
	if (parse_variables())
	{
		return -1;
	}
	
	((optimizer*)kuna_optimizer)->make_model();
	
	if (parse_knowledge())
	{
		return -1;
	}
	
	((optimizer*)kuna_optimizer)->calculate_payoff();
	
	if (parse_commands())
	{
		return -1;
	}
	
	/*
	do 
	{
		i = lex();
		std::cout << "got " << get_token_name(i) << "\n";
		
		if (i == T_STRING_LITERAL || i == T_IDENTIFIER)
		{
			std::cout << yylval.value_string << "\n";
		}
	}
	while (i!=0);
	*/
    return 0;
}


int kuna_cpp_parse_error_to_stderr(class_with_scanner * data)
{
	cpp_parser p(data->scanner,data->optimizer, &std::cerr);
	
	return p.parse();
}

int kuna_cpp_parse_error_to_stream(class_with_scanner * data, std::ostream * s)
{
	cpp_parser p(data->scanner,data->optimizer, s);
	
	return p.parse();
}
