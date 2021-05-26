

EXTRA_DIST = *.svarog *.txt

test1:
	svarog example1_dorban.svarog < example1_dorban_input1.txt
	
test2:
	svarog example1_dorban.svarog < example1_dorban_input2.txt
	
test3:
	svarog example1_dorban.svarog < example1_dorban_input3.txt

test4:
	svarog example3_dorban.svarog < example3_dorban_input1.txt

test5:
	svarog example3_dorban.svarog < example3_dorban_input2.txt

# it is the same like test5, but with precalculated knowledge - much faster
# even though the depth is set to 4
test6:
	svarog example15_precalculated4_with_bases.svarog < example10_dorban_input1.txt
