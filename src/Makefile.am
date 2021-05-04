EXTRA_DIST = parser.y

bin_PROGRAMS = svarog svarog-daemon svarog-dummy-client svarog-merge

svarog_SOURCES = kuna.cc

svarog_daemon_SOURCES = daemon.cc

svarog_dummy_client_SOURCES = dummy_client.cc

svarog_merge_SOURCES = svarog_merge.cc

lib_LTLIBRARIES = libsvarog.la

libsvarog_la_SOURCES = scanner.l cpp_parser.cc optimizer.cc kuna_c.cc collection_of_variables.cc \
	collection_of_values.cc variable.cc function.cc statement_if.cc collection_of_functions.cc \
	statement_return.cc knowledge_action.cc expression_function_call.cc knowledge_case.cc \
	knowledge_probability_case.cc action.cc collection_of_actions.cc state.cc visible_state.cc \
	collection_of_visible_states.cc command_loop.cc belief.cc clause_requires.cc \
	command_cout_knowledge.cc logical_expression_not.cc logical_expression_and.cc \
	logical_expression_or.cc clause_illegal.cc \
	command_cout_visible_states.cc command_cout_states.cc \
	logical_expression_equality.cc value_or_something_else.cc stack.cc \
	expression_initial_variable_value.cc expression_terminal_variable_value.cc \
	expression_value.cc knowledge_impossible.cc command_cout_perkun.cc command_test.cc \
	command_test_with_query.cc knowledge_payoff.cc configuration.cc \
	configuration_scanner.l configuration_parser.cc command_input.cc command_action.cc \
	command_belief.cc command_solve.cc command_cout_result.cc \
	command_cout_precalculate.cc command_cout_estimate_beliefs.cc \
	knowledge_precalculated.cc query.cc

svarog_LDADD = libsvarog.la @LIBREADLINE@ 

svarog_daemon_LDADD = libsvarog.la @LIBREADLINE@ 

svarog_merge_LDADD = libsvarog.la @LIBREADLINE@

AM_CXXFLAGS = @CXXFLAGS@ \
	-I.. -I../inc 

parser.tab.h: parser.y
	bison -d $^
	
lex.svarog.c: scanner.l
	flex $^
	
lex.svarog_config.c: configuration_scanner.l
	flex $^

scanner.c: lex.svarog.c
	cp $^ $@

configuration_scanner.c: lex.svarog_config.c
	cp $^ $@

scanner.lo: parser.tab.h

configuration_scanner.lo: parser.tab.h

clean-local:
	rm -f parser.tab.h
	
