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

#ifndef SVAROG_H
#define SVAROG_H

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <map>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace svarog
{
	
/**
 * Values are objects representing the variable values.
 */
class value
{
private:
	const std::string name;
public:
	value(const std::string & n): name(n) {}

	const std::string & get_name() const { return name; }
};


/** A variable represents a signal for the player.
 * - input variable - a visible signal
 * - hidden variable - an invisible signal
 * - output variable - an output signal (depending on the player)
 */
class variable
{
private:
	const std::string name;
	
protected:
	std::list<const value*> list_of_possible_values;
	
public:
	variable(const std::string & n): name(n) {}
	virtual bool get_is_input_variable() const { return false; }
	virtual bool get_is_hidden_variable() const { return false; }
	virtual bool get_is_output_variable() const { return false; }
	virtual const std::string get_type_name() const = 0;
	
	const std::string & get_name() const { return name; }
	
	void add_possible_value(const value * v) 
	{ 
		list_of_possible_values.push_back(v); 
	}
	const std::list<const value*> & get_list_of_possible_values() const { return list_of_possible_values; }
	
	bool get_can_have_value(const value * v) const;
	
	virtual void report_xml(std::ostream & s) const = 0;
	
	virtual void report_kuna(std::ostream & s) const = 0;
};


/**
 * An input variable - variable visible to the agent.
 */
class input_variable: public variable
{
public:
	input_variable(const std::string & n): variable(n) {}
	virtual bool get_is_input_variable() const { return true; }
	virtual const std::string get_type_name() const { return "input"; }
	virtual void report_xml(std::ostream & s) const
	{
		s << "<kuna:input_variable name=\"" << get_name() << "\">\n";
		s << "<kuna:possible_values>\n";
		for (std::list<const value*>::const_iterator i(list_of_possible_values.begin());
			 i != list_of_possible_values.end(); i++)
			 {
				 s << "<kuna:value name=\"" << (*i)->get_name() << "\" />\n";
			 }
		s << "</kuna:possible_values>\n";
		s << "</kuna:input_variable>\n";
	}
	
	virtual void report_kuna(std::ostream & s) const
	{
		s << "input variable " << get_name() << ":{";
		bool first = true;
		for (std::list<const value*>::const_iterator i(list_of_possible_values.begin());
			 i != list_of_possible_values.end(); i++)
		{
			if (!first) s << ",";
			s << (*i)->get_name();
			first = false;
		}
		s << "}";
	}
};

/**
 * An output variable - variable controlled by the agent.
 */
class output_variable: public variable
{
public:
	output_variable(const std::string & n): variable(n) {}	
	virtual bool get_is_output_variable() const { return true; }
	virtual const std::string get_type_name() const { return "output"; }
	virtual void report_xml(std::ostream & s) const
	{
		s << "<kuna:output_variable name=\"" << get_name() << "\">\n";
		s << "<kuna:possible_values>\n";
		for (std::list<const value*>::const_iterator i(list_of_possible_values.begin());
			 i != list_of_possible_values.end(); i++)
			 {
				 s << "<kuna:value name=\"" << (*i)->get_name() << "\" />\n";
			 }
		s << "</kuna:possible_values>\n";
		s << "</kuna:output_variable>\n";
	}
	virtual void report_kuna(std::ostream & s) const
	{
		s << "output variable " << get_name() << ":{";
		bool first = true;
		for (std::list<const value*>::const_iterator i(list_of_possible_values.begin());
			 i != list_of_possible_values.end(); i++)
		{
			if (!first) s << ",";
			s << (*i)->get_name();
			first = false;
		}
		s << "}";
	}
};

/**
 * A hidden variable - variable invisible to the agent.
 */
class hidden_variable: public variable
{
public:
	hidden_variable(const std::string & n): variable(n) {}
	virtual bool get_is_hidden_variable() const { return true; }
	virtual const std::string get_type_name() const { return "hidden"; }
	virtual void report_xml(std::ostream & s) const
	{
		s << "<kuna:hidden_variable name=\"" << get_name() << "\">\n";
		s << "<kuna:possible_values>\n";
		for (std::list<const value*>::const_iterator i(list_of_possible_values.begin());
			 i != list_of_possible_values.end(); i++)
			 {
				 s << "<kuna:value name=\"" << (*i)->get_name() << "\" />\n";
			 }
		s << "</kuna:possible_values>\n";
		s << "</kuna:hidden_variable>\n";
	}
	
	virtual void report_kuna(std::ostream & s) const
	{
		s << "hidden variable " << get_name() << ":{";
		bool first = true;
		for (std::list<const value*>::const_iterator i(list_of_possible_values.begin());
			 i != list_of_possible_values.end(); i++)
		{
			if (!first) s << ",";
			s << (*i)->get_name();
			first = false;
		}
		s << "}";
	}

};


/**
 * Collection of values contains a vector of values.
 * Some other classes must be aware of it.
 */
class collection_of_values
{
private:
	std::vector<value*> vector_of_values;
public:
	collection_of_values& operator<<(value * v) { vector_of_values.push_back(v); return *this; }
	~collection_of_values();
	
	std::vector<value*>& get_vector_of_values() { return vector_of_values; }

	const std::vector<value*>& get_vector_of_values() const { return vector_of_values; }

	value* get_value(const char * v);
};
	
	/**
 * This is a collection of variables. Some other classes must be aware of it.
 */
class collection_of_variables
{
private:
	std::vector<variable*> vector_of_variables;
public:
	~collection_of_variables();
	collection_of_variables& operator<<(variable* v) { vector_of_variables.push_back(v); return *this; }
	std::vector<variable*> & get_vector_of_variables() { return vector_of_variables; }

	const std::vector<variable*> & get_vector_of_variables() const { return vector_of_variables; }

	variable * get_variable(const char * v);
	
	const variable * get_variable(const char * v) const;
	
};

	class optimizer;

    /**
    * An auxilliary class derived from a map<variable*,value*>.
    * It is used to manipulate the queries.
    * */
    class query: public std::map<variable*, value*>
    {
		private:
        std::vector<variable*> & vector_of_variables;
        
        std::vector<value*> & vector_of_values;
		
		std::list<variable*> list_of_query_variables; // not owned
		
		public:
		query(std::vector<variable*>& v, std::vector<value*>& w);
			
		void assert_query_fact(variable* v, value * w);

        void report_kuna(std::ostream & s) const;
		
		query * get_copy(optimizer & target) const;
		
		bool get_equal(const query & q) const;
	};

	
	class value_or_something_else
	{
		public:
		enum mode_type { NONE, STRING_LITERAL, VALUE, VARIABLE };
		private:
		mode_type mode;
		std::string string_literal;
		value * my_value;
		variable * my_variable;
		
		std::string name;
		
		public:
		value_or_something_else(): mode(NONE) {}
		void set_mode(mode_type m) { mode = m; }
		void set_value(value * v) { my_value = v; }
		void set_variable(variable * v) { my_variable = v; }
		void set_string_literal(const std::string & l) { string_literal = l; }
		void set_name(std::string & n) { name = n; }
		
		mode_type get_mode() const { return mode; }
		const std::string & get_string_literal() const { return string_literal; }
		
		value * get_value() { return my_value; }
		variable * get_variable() { return my_variable; }
		
		std::string & get_name() { return name; }
		
		void report_kuna(std::ostream & s) const;
	};
	
	
	class expression
	{
		private:
		int line_number;
		public:
		expression(int ln): line_number(ln) {}
			
		virtual ~expression() {}
		
		virtual void report_kuna(std::ostream & s) const = 0;
		
		virtual void get_value(optimizer & o, value_or_something_else & target) const = 0;
		
		int get_line_number() const { return line_number; }
	};
	
	class expression_value: public expression
	{
		private:
		const std::string value_name;
		public:
		expression_value(const std::string & n, int ln): expression(ln), value_name(n) {}
		virtual void report_kuna(std::ostream & s) const
		{
			s << value_name;
		}
		
		virtual void get_value(optimizer & o, value_or_something_else & target) const;
	};
	
	class expression_function_call: public expression
	{
		private:
		const std::string function_name;
		std::vector<expression*> vector_of_arguments;
		public:
		expression_function_call(const std::string & f, int ln): expression(ln), function_name(f) {}
		virtual ~expression_function_call();
		
		virtual void report_kuna(std::ostream & s) const;
		
		void add_argument(expression *e);
		
		virtual void get_value(optimizer & o, value_or_something_else & target) const;
		
		const std::vector<expression*> & get_vector_of_arguments() const { return vector_of_arguments; }
	};
	
	class expression_initial_variable_value: public expression
	{
		private:
		const std::string variable_name;
		public:
		expression_initial_variable_value(const std::string & n, int ln): expression(ln), variable_name(n) {}
		virtual void report_kuna(std::ostream & s) const
		{
			s << "initial value " << variable_name;
		}
		virtual void get_value(optimizer & o, value_or_something_else & target) const;
	};
	
	class expression_terminal_variable_value: public expression
	{
		private:
		const std::string variable_name;
		public:
		expression_terminal_variable_value(const std::string & n, int ln): expression(ln), variable_name(n) {}
		virtual void report_kuna(std::ostream & s) const
		{
			s << "terminal value " << variable_name;
		}
		virtual void get_value(optimizer & o, value_or_something_else & target) const;
	};
	
	class expression_string_literal: public expression
	{
		private:
		const std::string literal;
		public:
		expression_string_literal(const std::string & l, int ln): expression(ln), literal(l) {}
		virtual void report_kuna(std::ostream & s) const
		{
			s << "string literal " << literal;
		}
		virtual void get_value(optimizer & o, value_or_something_else & target) const
		{
			target.set_mode(value_or_something_else::STRING_LITERAL);
			target.set_string_literal(literal);
		}
	};
	
	
	class logical_expression
	{
		private:
		int line_number;
		public:
		logical_expression(int ln): line_number(ln) {}
			
		virtual ~logical_expression() {}
		
		virtual void report_kuna(std::ostream & s) const = 0;
		
		virtual bool get_value(optimizer & o) const = 0;
		
		int get_line_number() const { return line_number; }
	};
	
	class logical_expression_not: public logical_expression
	{
		private:
		logical_expression *inner;
		public:
		logical_expression_not(logical_expression *i, int ln): logical_expression(ln), inner(i) {}
		virtual ~logical_expression_not();
			
		virtual void report_kuna(std::ostream & s) const;
		
		virtual bool get_value(optimizer & o) const;
	};
	
	class logical_expression_and: public logical_expression
	{
		private:
		logical_expression * left, * right;
		public:
		logical_expression_and(logical_expression *l, logical_expression *r, int ln): logical_expression(ln), left(l), right(r)
		{
		}
		
		virtual ~logical_expression_and();
		
		virtual void report_kuna(std::ostream & s) const;
		
		virtual bool get_value(optimizer & o) const;
	};
	
	class logical_expression_or: public logical_expression
	{
		private:
		logical_expression * left, * right;
		public:
		logical_expression_or(logical_expression *l, logical_expression *r, int ln): logical_expression(ln), left(l), right(r)
		{
		}
		
		virtual ~logical_expression_or();
		
		virtual void report_kuna(std::ostream & s) const;
		
		virtual bool get_value(optimizer & o) const;
	};
	
	class logical_expression_equality: public logical_expression
	{
		private:
		expression *left,*right;
		public:
		logical_expression_equality(expression *l, expression *r, int ln):
			logical_expression(ln), left(l), right(r) {}
		virtual ~logical_expression_equality()
		{
			delete left;
			delete right;
		}
		virtual void report_kuna(std::ostream & s) const
		{
			if (left) left->report_kuna(s);
			s << "==";
			if (right) right->report_kuna(s);
		}
		
		virtual bool get_value(optimizer & o) const;
	};
	
	
	class statement
	{
		private:
		public:
		virtual ~statement() {}
		
		virtual void report_kuna(std::ostream & s) const = 0;
		
		virtual void execute(optimizer & o, std::vector<statement*>::const_iterator & pc, const std::vector<statement*>::const_iterator & end) const = 0;
		
		virtual void set_stack_frame_size(int s) = 0;
	};
		
	class statement_return: public statement
	{
		private:
		expression * inner; // owned
		
		int stack_frame_size;
		
		public:
		statement_return(expression * e): inner(e) {}
		virtual ~statement_return()
		{
			delete inner;
		}
		virtual void report_kuna(std::ostream & s) const;
		virtual void execute(optimizer & o, std::vector<statement*>::const_iterator & pc, const std::vector<statement*>::const_iterator & end) const;
		
		virtual void set_stack_frame_size(int s) { stack_frame_size = s; }
	};
	
	class statement_if: public statement
	{
		private:
		logical_expression * my_logical_expression; // owned
		
		std::vector<statement*> vector_of_statements; // owned
		
		public:
		statement_if(): my_logical_expression(NULL) {}
		
		void set_condition(logical_expression * c) { my_logical_expression = c; }
		
		void add_statement(statement * s);
		
		virtual ~statement_if();
		
		virtual void report_kuna(std::ostream & s) const;
		
		virtual void execute(optimizer & o, std::vector<statement*>::const_iterator & pc, const std::vector<statement*>::const_iterator & end) const;
		
		virtual void set_stack_frame_size(int s);
	};
	
	class statement_expression: public statement
	{
		private:
		expression * inner; // owned
		public:
		statement_expression(expression * i): inner(i) {}
		virtual ~statement_expression()
		{
			delete inner;
		}
		virtual void report_kuna(std::ostream & s) const
		{
			inner->report_kuna(s);
			s << ";";
		}
		virtual void execute(optimizer & o, std::vector<statement*>::const_iterator & pc, const std::vector<statement*>::const_iterator & end) const
		{
		}
		
		virtual void set_stack_frame_size(int s) {}
	};
		
	class function
	{
		public:
		class parameter
		{
			public:
			virtual ~parameter() {}
			virtual void report_kuna(std::ostream & s) const = 0;
			
			virtual std::string get_name() const = 0;
		};
		
		class parameter_value: public parameter
		{
			private:
			const std::string value_name;
			public:
			parameter_value(const std::string & n): value_name(n) {}
			virtual void report_kuna(std::ostream & s) const
			{
				s << "value " << value_name;
			}
			virtual std::string get_name() const 
			{
				return value_name;
			}
		};
		
		class parameter_variable: public parameter
		{
			private:
			const std::string variable_name;
			public:
			parameter_variable(const std::string & n): variable_name(n) {}
			virtual void report_kuna(std::ostream & s) const
			{
				s << "variable " << variable_name;
			}
			virtual std::string get_name() const
			{
				return variable_name;
			}
		};
		
		
		private:
		const std::string name;
		
		std::vector<parameter*> vector_of_parameters; // owned
		
		std::vector<statement*> vector_of_statements; // owned
		
		public:
		function(const std::string & n): name(n)
		{
		}
		
		virtual ~function();
		
		const std::string& get_name() const { return name; }
		
		void add_parameter(parameter * p);
		
		void add_statement(statement * s);
		
		void report_kuna(std::ostream & s) const;
		
		void execute(optimizer & o);
		
		const std::vector<parameter*> & get_vector_of_parameters() const { return vector_of_parameters; }
	};
	
	class collection_of_functions
	{
		private:
		std::vector<function*> vector_of_functions; // owned
		
		std::map<std::string,function*> map_function_name_to_function;
		
		public:
		~collection_of_functions();	
		
		void add_function(function * f) 
		{ 
			vector_of_functions.push_back(f); 
			map_function_name_to_function.insert(std::pair<std::string,function*>(f->get_name(), f));
		}
		
		std::vector<function*> & get_vector_of_functions() { return vector_of_functions; }
		
		const std::vector<function*> & get_vector_of_functions() const { return vector_of_functions; }
		
		function* get_function(const std::string & n) { return map_function_name_to_function.at(n); }
	};
	
	
	class clause
	{
		public:
		virtual ~clause() {}
		
		virtual void report_kuna(std::ostream & s) const = 0;
		
		virtual bool get_match(optimizer & o) const = 0;
		
		virtual bool get_is_illegal() const { return false; }
	};
	
	/**
	 * In the classes knowledge_case and knowledge_probability_case these clauses determine
	 * the precondition.
	 */
	class clause_requires: public clause
	{
		private:
		logical_expression * inner;
		
		public:
		clause_requires(logical_expression *i): inner(i) {}
		virtual ~clause_requires();
		
		virtual void report_kuna(std::ostream & s) const;
		
		virtual bool get_match(optimizer & o) const;
	};
		
	class clause_illegal: public clause
	{
		private:
		public:
		virtual void report_kuna(std::ostream & s) const;
		
		virtual bool get_match(optimizer & o) const;
		
		virtual bool get_is_illegal() const { return true; }
	};
	
	class belief;
	class action;
	class visible_state;
	
	/**
	 * The precalculated knowledge is produced by the command
	 * cout << precalculate(<depth>,<granularity>) << eol;
	 * 
	 * It contains some rules based on the analysis of the specification.
	 * This should improve the performance.
	 */
	class knowledge_precalculated
	{
		public:
		class on_visible_state
		{
			public:
			class on_belief
			{
			public:
				class belief_case
				{
				private:
					const float probability;
					query * my_query;	// owned
				public:
					belief_case(float p, query * q): probability{p}, my_query{q} {}
					~belief_case()
					{
						delete my_query;
					}
					const query* get_query() const { return my_query; }
					float get_probability() const { return probability; }
					
					void report_kuna(std::ostream & s) const;
					
					belief_case * get_copy(optimizer & target) const;
				};
			private:
				std::list<belief_case*> list_of_belief_cases; // owned
				query * my_action_query; // owned
				const action * my_action; // not owned
			public:
				on_belief(): my_action_query{nullptr}, my_action{nullptr} {}
				~on_belief();
				void add_belief_case(belief_case * i) { list_of_belief_cases.push_back(i); }
				void set_action_query(query * q) { my_action_query = q; }
				void set_action(const action * a) { my_action = a; }
				
				const std::list<belief_case*>& get_list_of_belief_cases() const 
				{ return list_of_belief_cases; }
				const query * get_action_query() const { return my_action_query; }
				
				float get_distance(const belief & b) const;
				
				const action* get_action() const { return my_action; }
				
				void report_kuna(std::ostream & s) const;
				
				on_belief* get_copy(optimizer & target) const;
			};
			private:
			query * my_query; // owned
			
			std::list<on_belief *> list_of_objects_on_belief; // owned
			
			float amount_of_possible_states, max_amount_of_beliefs;
			
			bool too_complex;
			
			public:
				
			on_visible_state(query * q): 
				my_query{q}, 
				amount_of_possible_states{0.0f},
				max_amount_of_beliefs{0.0f},
				too_complex{false} {}
			~on_visible_state();
			
			void add_on_belief(on_belief * i) { list_of_objects_on_belief.push_back(i); }
			
			const std::list<on_belief*> & get_list_of_objects_on_belief() const
			{ return list_of_objects_on_belief; }
			
			const query* get_query() const { return my_query; }

			void report_kuna(std::ostream & s) const;
			
			on_visible_state* get_copy(optimizer & target) const;
			
			void set_amount_of_possible_states(float a) { amount_of_possible_states = a; }
			
			void set_max_amount_of_beliefs(float m) { max_amount_of_beliefs = m; }
			
			void set_too_complex(bool t) { too_complex = t; }
		};
		
		private:
		std::list<on_visible_state*> list_of_objects_on_visible_state; // owned
		const int depth, granularity;
		
		std::map<const visible_state *, on_visible_state*> map_visible_state_to_object_on_visible_state;
		
		public:
		knowledge_precalculated(int d, int g): depth{d}, granularity{g} {}
		~knowledge_precalculated();
		
		int get_depth() const { return depth; }
		
		int get_granularity() const { return granularity; }
		
		void add_on_visible_state(on_visible_state * o) 
		{ list_of_objects_on_visible_state.push_back(o); }
		
		const std::list<on_visible_state*> & get_list_of_objects_on_visible_state() const
		{ return list_of_objects_on_visible_state; }
		
		/**
		 * This method populates the map_visible_state_to_object_on_visible_state,
		 * it also stores the action pointers in the on_belief objects.
		 * This makes using the knowledge_precalculated faster.
		 * It is called by the cpp_parser.
		 */
		void learn(optimizer & o);
		
		/**
		 * Return the on_visible_state object corresponding with the visible state.
		 * This function can return nullptr.
		 */
		const on_visible_state* get_object_on_visible_state(const visible_state & x) const;
		
		/**
		 * Merge the knowledge precalculated.
		 */
		void merge(const knowledge_precalculated & k, optimizer & target);
		
		void report_kuna(std::ostream & s) const;
	};
	
	class knowledge_impossible
	{
		private:
		std::list<clause*> list_of_clauses; // owned
			
		const std::string name;
		public:
		knowledge_impossible(const std::string & n): name(n) {}
		
		~knowledge_impossible();

		const std::string & get_name() const { return name; }
		
		void add_clause(clause *);
		
		bool get_possible(optimizer & o);
		
		void report_kuna(std::ostream & s) const;
	};
	
	
	class knowledge_payoff
	{
		private:
		const std::string name;
		const float payoff;

		std::list<clause*> list_of_clauses; // owned	
		
		public:
		knowledge_payoff(const std::string & n, float f): name(n), payoff(f) {}
		~knowledge_payoff();
		
		void add_clause(clause * c);
		
		void report_kuna(std::ostream & s) const;
		
		float get_payoff() const { return payoff; }
		
		bool get_match(optimizer & o);
	};
		
	class knowledge_action
	{
		public:
		class knowledge_case
		{
			public:
			class knowledge_probability_case
			{
				private:
				float probability;
				
				std::list<clause*> list_of_clauses; // owned	
				
				const std::string case_name;

				public:
				knowledge_probability_case(const std::string & n, float p): 
					case_name(n), probability(p) {}
				~knowledge_probability_case();
								
				void report_kuna(std::ostream & s) const;
			
				void add_clause(clause *);
				
				float get_probability() const { return probability; }
				
				const std::list<clause*>& get_list_of_clauses() const { return list_of_clauses; }
				
				const std::string & get_name() const { return case_name; }
			};
			
			private:
			std::list<knowledge_probability_case*> list_of_probability_cases; // owned
			
			std::list<clause*> list_of_clauses; // owned
			
			const std::string case_name;
			
			public:
			knowledge_case(const std::string & n): case_name(n) {}
			
			~knowledge_case();
			
			void report_kuna(std::ostream & s) const;
			
			void add_clause(clause *);
			
			void add_probability_case(knowledge_probability_case *c);
			
			const std::list<clause*> & get_list_of_clauses() const { return list_of_clauses; }
			
			const std::list<knowledge_probability_case*> & get_list_of_probability_cases() const 
			{
				return list_of_probability_cases;
			}
			
			const std::string & get_name() const { return case_name; }
			
			bool get_contains_illegal() const;
			
		};
		private:
		query * my_query;	
		
		std::list<knowledge_case*> list_of_cases; // owned
		
		public:
		knowledge_action(query * q): my_query(q) {}
		
		~knowledge_action();
		
		void report_kuna(std::ostream & s) const;
		
		void add_case(knowledge_case *c);
		
		const query* get_query() const { return my_query; }
		
		const std::list<knowledge_case*> & get_list_of_cases() const { return list_of_cases; }
	};
	
	/**
	 * An action contains a map from an (output) variable to a value.
	 * It is aware of the collections: variables and values.
	 */
	class action
	{
		private:
		collection_of_variables & my_variables;
		collection_of_values & my_values;
	
		/**
		 * This is the map reflecting the action.
		 */
		std::map<variable*, value*> map_output_variable_to_value;
	
		public:
		action(collection_of_variables & v, collection_of_values & w):
			my_variables(v),
			my_values(w)
			{
			}
		
		const value * get_output_variable_value(variable * v) const { return map_output_variable_to_value.at(v); }
		
		void insert(variable * v, value * w) { map_output_variable_to_value.insert(std::pair<variable*,value*>(v, w)); }

		void report_kuna(std::ostream & s) const;
	
	
		bool get_match(const std::map<variable*, value*> & m) const;
	
		const std::map<variable*, value*>& get_map_output_variable_to_value() const
		{ return map_output_variable_to_value; }
	};
	
	
	/**
	 * A collection of actions - contains a list of actions.
	 * Some other classes must be aware of it.
	 */
	class collection_of_actions
	{
		private:
		collection_of_variables & my_variables;
		collection_of_values & my_values;
	
		std::list<action*> list_of_actions;
	
		bool get_allowed(const std::map<variable*, std::vector<value*>::const_iterator> & m) const;

		public:
		collection_of_actions(collection_of_variables & v, collection_of_values & w):
			my_variables(v), my_values(w) {}
		
		~collection_of_actions();
		
		void populate();
	
		const std::list<action*> & get_list_of_actions() const { return list_of_actions; }
	
		std::list<action*> & get_list_of_actions() { return list_of_actions; }
	
		action * get(const std::map<variable*, value*> & m);
	};
	
	class visible_state;
	
	/**
	 * A state is an item within a visible state. A state contains a map from a (hidden)
	 * variable to a value.
	 * It also contains a piece of model - a map from action to the probability distribution
	 * mapping states to float numbers.
	 */
	class state
	{
		private:
		collection_of_variables & my_variables;
		collection_of_values & my_values;
		visible_state & my_visible_state;
		collection_of_actions & my_actions;
		optimizer & my_optimizer;

		std::map<variable*, value*> map_hidden_variable_to_value;
	
		public:
		state(collection_of_variables & v, collection_of_values & w, visible_state & vs, collection_of_actions & a, optimizer & o):
				my_variables(v),
				my_values(w),
				my_visible_state(vs),
				my_actions(a),
				my_optimizer(o)
			{
			}
		
		const value * get_hidden_variable_value(variable * v) const
		{
			return map_hidden_variable_to_value.at(v);
		}

		const value * get_hidden_variable_value(const variable * v) const
		{
			return map_hidden_variable_to_value.at(const_cast<variable*>(v));
		}

		value * get_hidden_variable_value(variable * v)
		{
			return map_hidden_variable_to_value.at(v);
		}

		void insert(variable * v, value * w) { map_hidden_variable_to_value.insert(std::pair<variable*,value*>(v, w)); }
		
		void report_kuna(std::ostream & s) const;
	
	
		bool get_match(const std::map<variable*, value*> & m) const;
	
	
		visible_state & get_visible_state() { return my_visible_state; }
	
		const visible_state & get_visible_state() const { return my_visible_state; }
	
		void populate_map(std::map<variable*, value*> & target) const;

		void populate_map(std::map<variable*, value*> & target);
		
		bool get_possible() const;
		
		const std::string get_why_is_impossible() const;
	};
	
	class collection_of_visible_states;
	
	/**
	 * A visible state represents a map from input variable to values and is at the same
	 * time a collection of states.
	 */
	class visible_state
	{
		private:
		collection_of_variables & my_variables;
		collection_of_values & my_values;
		collection_of_actions & my_actions;
		collection_of_visible_states & my_visible_states;
		optimizer & my_optimizer;
		
		std::list<state*> list_of_states; // owned
		std::map<variable*,value*> map_input_variable_to_value;
		
		bool get_allowed(const std::map<variable*, std::vector<value*>::const_iterator> & m) const;
	
		public:
		visible_state(collection_of_variables & v, collection_of_values & w, collection_of_actions & a, collection_of_visible_states & q, optimizer & o): 
			my_variables(v),
			my_values(w),
			my_actions(a),
			my_visible_states(q),
			my_optimizer(o)
			{
			}
		
		~visible_state();
		
		void insert(variable * v, value * w) { map_input_variable_to_value.insert(std::pair<variable*,value*>(v, w)); }	
		
		void populate();
	
		void report_kuna(std::ostream & s) const;
		
		bool get_match(const std::map<variable*, value*> & m) const;
	
		state * get(const std::map<variable*, value*> & m);

		const state * get(const std::map<variable*, value*> & m) const;
	
		const value* get_input_variable_value(variable* v) const { return map_input_variable_to_value.at(v); }
		
		const value* get_input_variable_value(const variable* v) const { return map_input_variable_to_value.at(const_cast<variable*>(v)); }
	
		value* get_input_variable_value(variable* v) { return map_input_variable_to_value.at(v); }	
	
		const std::list<state*> & get_list_of_states() const { return list_of_states; }
	
		std::list<state*> & get_list_of_states() { return list_of_states; }
	
		float get_payoff() const;
	
		const collection_of_visible_states & get_visible_states() const { return my_visible_states; }

		collection_of_visible_states & get_visible_states() { return my_visible_states; }
		
		int get_amount_of_states() const { return list_of_states.size(); }
		
		int get_amount_of_possible_states() const;
							
		bool get_is_illegal(const action * a) const;
		
		bool get_is_illegal_for_case(const action * a, knowledge_action::knowledge_case * j) const;

	};

	
	/**
	 * A collection of visible states. It contains a list of visible states and
	 * the payoff function mapping visible states to float.
	 */
	class collection_of_visible_states
	{
		private:
		collection_of_variables & my_variables;
		collection_of_values & my_values;
		collection_of_actions & my_actions;
	
		std::list<visible_state*> list_of_visible_states;
	
		std::map<const visible_state*,float> map_visible_state_to_payoff_value;
	
		bool get_allowed(const std::map<variable*, std::vector<value*>::const_iterator> & m) const;
	
		public:
		collection_of_visible_states(collection_of_variables & v, collection_of_values & w, collection_of_actions & a);
		~collection_of_visible_states();
	
		void populate(optimizer & o);
	
		const visible_state * get(const std::map<variable*, value*> & m) const;

		visible_state * get(const std::map<variable*, value*> & m);	
	
		float get_payoff(const visible_state * s)
		{
			if (map_visible_state_to_payoff_value.find(s) == map_visible_state_to_payoff_value.end())
			{
				return 0.0f;
			}
			
			return map_visible_state_to_payoff_value.at(s);
		}
	
		void set_payoff(const visible_state *s, float p) 
		{ 
			if (map_visible_state_to_payoff_value.find(s) == map_visible_state_to_payoff_value.end())
				map_visible_state_to_payoff_value.insert(std::pair<const visible_state *,float>(s, p)); 
			else
				map_visible_state_to_payoff_value[s] = p;
		}
	
		void set_payoff(const std::map<variable*, value*> & m, float p)
		{
			const visible_state * s = get(m);
			if (s == NULL)
			{
				throw std::runtime_error("query failed");
			}
			set_payoff(s, p);
		//std::cout << "SET PAYOFF " << p << "\n";
		}
	
		void report_kuna(std::ostream & s) const;
	
		const std::list<visible_state*> & get_list_of_visible_states() const { return list_of_visible_states; }
	
		std::list<visible_state*> & get_list_of_visible_states() { return list_of_visible_states; }	
		
		void calculate_payoff(optimizer & o);
	};
	
	
	/**
	 * Probability distribution over a set of possible states within a given
	 * visible state.
	 */
	class belief
	{
		private:
		visible_state & my_visible_state;
		std::map<state *, float> map_state_to_probability;
		optimizer & my_optimizer;
		
		public:
		belief(visible_state & v, optimizer & o): my_visible_state(v), my_optimizer(o) {}
		float get_probability(state * s) const;
	
		void set_probability(state * s, float p);
	
		void set_probability(const std::map<variable*, value*> & query, float p) 
		{
			state * s = my_visible_state.get(query);
			if (s == NULL)
			{
				throw std::runtime_error("query failed");
			}
			set_probability(s, p);
		}
	
		void report_kuna(std::ostream & s) const;
	
		void make_uniform();
	
		const visible_state & get_visible_state() const { return my_visible_state; }
	};

	
	class command
	{
		public:
		virtual ~command() {}
		virtual void execute(optimizer & o) const = 0;
		
		/**
		 * The interactive commands must not be sent to the svarog daemons.
		 */
		virtual bool get_is_interactive() const { return false; }
	};
	
	
	class command_belief: public command
	{
	public:
		class belief_case
		{
		private:
			query * my_query; // owned
			float my_probability;
		public:
			belief_case(query * q, float p): my_query(q), my_probability(p) {}
			~belief_case() { delete my_query; }
			
			const query* get_query() const { return my_query; }
			float get_probability() const { return my_probability; }
		};
	private:
		std::list<belief_case*> list_of_belief_cases; // owned
		
	public:
		virtual ~command_belief();
		virtual void execute(optimizer & o) const;
		void add(belief_case * c) { list_of_belief_cases.push_back(c); }
	};
	
	/**
	 * This command is used to determine the next input.
	 */
	class command_input: public command
	{
		private:
		query * my_query; // owned
		
		public:
		command_input(query * q): my_query(q) {}
		
		virtual ~command_input() 
		{ 
			delete my_query;
		}
		virtual void execute(optimizer & o) const;
	};

	/**
	 * This command is used to determine the next action.
	 */	
	class command_action: public command
	{
		private:
		query * my_query; // owned
		
		public:
		command_action(query * q): my_query(q) {}
		
		virtual ~command_action() 
		{ 
			delete my_query;
		}
		virtual void execute(optimizer & o) const;
	};
	
	class command_solve: public command
	{
	private:
		const int depth;
	public:
		command_solve(int d): depth{d} {}
		virtual void execute(optimizer & o) const;
	};
	
	class command_test: public command
	{
		private:
		void execute_for(optimizer & o, const visible_state * i, const state * j) const;
		public:
		virtual void execute(optimizer & o) const;
	};
	
	class command_test_with_query: public command
	{
		private:
		query * my_query; // owned
		
		void execute_for(optimizer & o, const visible_state * i, const state * j) const;
		
		public:
		command_test_with_query(query * q): my_query(q) {}
		virtual ~command_test_with_query()
		{
			delete my_query;
		}
		virtual void execute(optimizer & o) const;		
	};
	
	class command_loop: public command
	{
		private:
		const int depth;
		
		public:
		command_loop(int d): depth(d) {}
		virtual void execute(optimizer & o) const;
		
		virtual bool get_is_interactive() const;
	};
	
	class command_cout_precalculate: public command
	{
		protected:
		const int depth, granularity;

		/**
		 * This limit sets the max amount of beliefs that are considered.
		 * If the amount of beliefs exceeds it the whole visible state
		 * will be marked as "too complex" and not precalculated.
		 */
		static constexpr int amount_of_beliefs_limit = 1024;
		
		float get_amount_of_possible_states(const visible_state & x, optimizer & o) const;
		
		float get_max_amount_of_beliefs(const visible_state & x, optimizer & o) const;
		
		bool get_any_state_is_possible(const visible_state & x, optimizer & o) const;
		
		void dump_map_state_to_count(const visible_state & x, const std::map<state*,int> & map_state_to_count) const;
		
		void create_next_belief(belief & b, std::map<state*,int> & map_state_to_count, bool & done, bool & skip, optimizer & o) const;
		
		virtual void on_belief(belief & b, optimizer & o) const;

		public:
		command_cout_precalculate(int d, int g): 
			depth{d}, granularity{g} {} 
		virtual void execute(optimizer & o) const;
	};

	class command_cout_estimate_beliefs: public command_cout_precalculate
	{
		protected:
		virtual void on_belief(belief & b, optimizer & o) const override;

		public:
		command_cout_estimate_beliefs(int d, int g): command_cout_precalculate{d, g} {}
	};
	
	class command_cout_knowledge: public command
	{
		public:
		virtual void execute(optimizer & o) const;
	};
	
	class command_cout_visible_states: public command
	{
		public:
		virtual void execute(optimizer & o) const;
	};

	class command_cout_states: public command
	{
		public:
		virtual void execute(optimizer & o) const;		
	};
	
	class command_cout_perkun: public command
	{
		public:
		virtual void execute(optimizer & o) const;
	};
	
	class command_cout_result: public command
	{
		public:
		virtual void execute(optimizer & o) const;
	};
	
	
	class stack
	{
		private:
		std::vector<value_or_something_else> vector_of_values_or_something_else;
		
		public:
		void push(value_or_something_else & v, std::string n) 
		{ 
			v.set_name(n);
			vector_of_values_or_something_else.push_back(v); 
		}
		int size() const { return vector_of_values_or_something_else.size(); }
		
		value_or_something_else pop() { 
			value_or_something_else t = vector_of_values_or_something_else[size()-1]; 
			vector_of_values_or_something_else.pop_back(); 
			return t; 
		}
		
		void report_kuna(std::ostream & s) const;
		
		value_or_something_else& operator[](int i) { return vector_of_values_or_something_else[i]; }
	};
	
	
	
	class configuration
	{
	private:
		class configuration_server
		{
		private:
			const std::string name, password;
			int port;
			
		public:
			configuration_server(const std::string & a, const std::string & p, int new_port): 
				name{a}, password{p}, port{new_port} {}
			
			const std::string & get_name() const { return name; }
			
			int get_port() const;
			
			const std::string & get_password() const { return password; }
		};
		
		std::vector<configuration_server> vector_of_configuration_servers;
		
	public:
		const std::string get_configuration_file_name() const;
		
		const std::string get_full_configuration_file_name() const;
		
		void make_configuration_file();
		
		int parse(const char * filename);
		
		void add_server(const std::string & a, int port, const std::string & p);
		
		bool get_has_servers() const { return vector_of_configuration_servers.size()>0; }
		
		int get_amount_of_servers() const { return vector_of_configuration_servers.size(); }
		
		const std::string & get_server_name(int i) const { return vector_of_configuration_servers.at(i).get_name(); }
		
		int get_server_port(int i) const 
		{ return vector_of_configuration_servers.at(i).get_port(); }
		
		const std::string & get_server_password(int i) const 
		{ return vector_of_configuration_servers.at(i).get_password(); }
	};

	
	/**
	 * This is the main class. It works as an optimizer - chooses the optimal actions
	 * given a belief and depth.
	 */
	class optimizer
	{
		private:
			
		/**
		 * When using servers (from the configuration) we use the tasks.
		 */
		class task
		{
		private:
			int sockfd;
			hostent * server;
			sockaddr_in serv_addr;
			const action * my_action;
			const std::string password;
			const std::string server_name;
			int port;
			
			static constexpr const int SVAROG_MAX_MESSAGE_FROM_SERVER = 256; 
			
			char buffer[SVAROG_MAX_MESSAGE_FROM_SERVER];
			
			int read_from_server();
			
		public:
			task(const std::string & new_server_name, int new_port, const std::string & pw, const action * a);
			~task();
			
			int write_and_run(const std::string & data);
			
			std::string get_response() { return std::string(buffer, read_from_server()); }
			
			const action * get_action() const { return my_action; }
			
			const std::string & get_password() const { return password; }
			
			const std::string & get_server_name() const { return server_name; }
			
			int get_port() const { return port; }
		};
		
		std::vector<task*> vector_of_tasks; // owned
		
		void add_task(task * t) { vector_of_tasks.push_back(t); }
		
		void clear_tasks();
		
		std::string get_partial_task_specification(const belief & b, int n, const action * a) const;
		
		int file_counter;	// to store the partial specification files
		
		std::string error_message;
		
		public:
		enum mode_type { DECLARING_VALUES, DECLARING_VARIABLES_VALUES };
		
		enum evaluating_expressions_mode_type { GET_MODEL, GET_IS_ILLEGAL, GET_POSSIBLE, GET_PAYOFF };
		
		private:
		mode_type mode;

		public:
		evaluating_expressions_mode_type evaluating_expressions_mode;
						
		public:
		std::list<knowledge_action*> list_of_knowledge_actions; // owned
		
		std::list<knowledge_impossible*> list_of_knowledge_impossibles; // owned
		
		std::list<knowledge_payoff*> list_of_knowledge_payoffs; // owned
		
		std::list<knowledge_precalculated*> list_of_knowledge_precalculated; // owned
		
		std::vector<command*> vector_of_commands; // owned
		
		std::list<variable*> list_of_input_variables; // not owned
		
		stack my_stack;
			
		collection_of_values w;
		
		collection_of_variables v;
		
		collection_of_functions f;
		
		collection_of_actions a;
		
		collection_of_visible_states vs;
				
		variable * e; // recently created variable (not owned), parsing time
		
		belief *current_belief, *former_belief; // beliefs (owned)
	
		const action * former_action; // former action (not owned)

		const state * get_model_state1, * get_model_state2; // not owned
		
		const visible_state * get_illegal_visible_state1; // not owned
		
		const state * get_possible_state1, * get_possible_state2; // not owned
		
		const visible_state * get_payoff_visible_state1; // not owned
		
		/**
		 * The initial input is set by the command_input.
		 * It is useful to solve sinle problems.
		 */
		const query * initial_input; // not owned
		
		const action * initial_action; // not owned
		
		belief * initial_belief; // owned
		
		float last_solution;
				
		static configuration my_configuration;
		
		optimizer();
		
		~optimizer();
		
		void set_initial_input(const query * ii) { initial_input = ii; }
		
		void set_initial_action(const action * a) { initial_action = a; }
		
		void set_initial_belief(belief * b);
		
		float get_last_solution() const { return last_solution; }
		
		void push(value_or_something_else & x, std::string n) { my_stack.push(x, n); }
		
		variable * get_variable(const char * n);
		
		value * get_value(const char * n);
				
		void declare_values();
		
		void declare_variables_values();
		
		void add_value(const char * s);
		
		void add_command(command * c);
		
		void create_input_variable(const char * s);

		void create_hidden_variable(const char * s);
		
		void create_output_variable(const char * s);

		int parse(const char * filename);
		
		int parse_buffer(const char * buffer);
		
		void add_knowledge_action(knowledge_action * a);
		
		void add_knowledge_impossible(knowledge_impossible * i);
		
		void add_knowledge_payoff(knowledge_payoff * p);
		
		void add_knowledge_precalculated(knowledge_precalculated * p);
		
		void make_model();
		
		void run();
		
		void report_kuna(std::ostream & s) const;
		
		/**
		 * Merge precalculated knowledge.
		 */
		void merge(const optimizer & o);
		
		/**
		 * This function fills the map m with the pairs (variable,value) corresponding with the input variable values.
		 * The flag eof can be optionally set to mark the end of file.
		 */
		virtual void get_input(std::map<variable*, value*> & m, bool & eof);
	
		/**
		 * This function executes the current action.
		 */
		virtual void execute(const action * a);
		
		/**
		 * Calculates the belief (probability distribution over a visible state)
		 * given a visible state as consequence.
		 */
		void populate_belief_for_consequence(const belief & b1, const action & a, const visible_state & vs, belief & target);
		
		/**
		 * This function throws an error. It can be overriden to operate
		 * more smoothly, for example make the target a uniform belief.
		 */
		virtual void on_surprise(const belief & b1, const action & a, const visible_state & vs, belief & target);

		float get_model_probability(const state * s1, const action &a, const state * s2);
		
		float get_model_probability_for_case(knowledge_action::knowledge_case * j, bool & found);
		
		float get_consequence_probability(const belief & b1, const action & a, const visible_state & vs);
		
		float get_payoff_expected_value_for_consequences(const belief & b1, int n, const action & a);
		
		const action * get_optimal_action(const belief & b, int n);
		
		const action * get_optimal_action_using_servers(const belief & b, int n);
		
		float solve(int n);
		
		virtual void set_apriori_belief(belief * b) const;
		
		virtual void print_initial_loop_message() const;
		
		virtual void print_belief(const belief & b) const;
		
		void calculate_payoff();
		
		/**
		 * The interactive commands are not allowed to be passed
		 * to the svarog daemons.
		 */
		bool get_contains_interactive_commands() const;
		
		void set_error_message(const std::string & m) { error_message = m; }
		
		const std::string & get_error_message() const { return error_message; }
	};
	
}

#endif
