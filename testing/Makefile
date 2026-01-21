RD = $(realpath ../res)
BD = $(realpath ../bin)
SD = $(realpath ../src)

SRC_FILES = ArgParsing.cpp

TST_FILES = ArgTableBuilder.cpp					\
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

OBJ_FILES = $(patsubst %.cpp,%.o,$(TST_FILES) $(SRC_FILES))
OBJ_PATHS = $(addprefix $(BD)/,$(OBJ_FILES))

###################################################################################################

all: compile link

compile:
	g++ -g -c -Wall -D DEBUG -std=c++17 $(SD)/$(SRC_FILES) $(TST_FILES)
	mv *.o $(BD)

link: 
	g++ $(OBJ_PATHS) -L$(RD) -lRandomizer -Wl,-rpath,$(RD)	\
					 -o $(BD)/argparsing

single:
	g++ -g -c -Wall -D DEBUG -std=c++17 $(FILE)

example:
	g++ -g -c -Wall -std=c++17 ArgParsingExample.cpp $(SD)/$(SRC_FILES)
	gcc -g -c -Wall -std=c17 ArgParsingCExample.c
	mv *.o $(BD)
	make -C $(SD)
	g++ $(BD)/ArgParsingExample.o $(BD)/ArgParsing.o -o $(BD)/aptest
	gcc $(BD)/ArgParsingCExample.o -L$(RD) -lArgParsing -Wl,-rpath,$(RD) -o $(BD)/aptest_c

clean:
	rm $(BD)/argparsing $(BD)/aptest* $(BD)/*.o *.o $(SD)*.gch
