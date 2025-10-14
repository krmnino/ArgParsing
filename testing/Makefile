RD = ../res/
BD = ../bin/
SD = ../src/

SRC_FILES = $(SD)ArgParsing.cpp					\
			ArgTableBuilder.cpp					\
			TestcaseBuilder.cpp					\
			ScenarioBuilder.cpp					\
			Validation.cpp						\
			Utils.cpp							\
			OK_Scenario.cpp						\
			MISSING_FIRST_DASH_Scenario.cpp		\
			MISSING_REQUIRED_ARG_Scenario.cpp	\
			UNKNOWN_ARGUMENT_Scenario.cpp		\
			REPEATED_ARGUMENT_Scenario.cpp		\
			MUST_BE_FLAG_Scenario.cpp			\
			BAD_NUMERIC_VALUE_Scenario.cpp		\
			EMPTY_ARG_LIST_Scenario.cpp			\
			VALID_FLAG_GROUP_Scenario.cpp		\
			INVALID_FLAG_GROUP_Scenario.cpp		\
			Main.cpp

OBJ_FILES = ArgParsing.o						\
			ArgTableBuilder.o					\
			TestcaseBuilder.o					\
			ScenarioBuilder.o					\
			Validation.o						\
			Utils.o  							\
			OK_Scenario.o						\
			MISSING_FIRST_DASH_Scenario.o		\
			MISSING_REQUIRED_ARG_Scenario.o		\
			UNKNOWN_ARGUMENT_Scenario.o			\
			REPEATED_ARGUMENT_Scenario.o		\
			MUST_BE_FLAG_Scenario.o				\
			BAD_NUMERIC_VALUE_Scenario.o		\
			EMPTY_ARG_LIST_Scenario.o			\
			VALID_FLAG_GROUP_Scenario.o			\
			INVALID_FLAG_GROUP_Scenario.o		\
			Main.o

###################################################################################################

all: compile link

compile:
	g++ -g -c -Wall -D DEBUG -std=c++17 $(SRC_FILES) 

link: 
	g++ $(OBJ_FILES) -L$(RD) -lRandomizer -Wl,-rpath,$(RD)	\
					 -o $(BD)argparsing

single:
	g++ -g -c -Wall -std=c++17 $(FILE) -D DEBUG

example:
	g++ -g -c -Wall -std=c++17 ArgParsingExample.cpp
	gcc -g -c -Wall -std=c17 ArgParsingCExample.c
	g++ ArgParsingExample.o -L$(RD) -lArgParsing -Wl,-rpath,$(RD)	\
							-o $(BD)aptest
	gcc ArgParsingCExample.o -L$(RD) -lArgParsing -Wl,-rpath,$(RD)	\
							-o $(BD)aptest_c

clean:
	rm $(BD)argparsing $(BD)argtest *.o ../src/*.gch
