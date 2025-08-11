RD = ../res/
BD = ../bin/
SD = ../src/

SRC_FILES = $(SD)ArgParsing.cpp		\
			ArgTableBuilder.cpp		\
			ScenarioBuilder.cpp		\
			Utils.cpp				\
			Main.cpp

OBJ_FILES = ArgParsing.o			\
			ArgTableBuilder.o		\
			ScenarioBuilder.o		\
			Utils.o  				\
			Main.o

all: compile link

compile:
	g++ -g -c -Wall -D DEBUG -std=c++17 $(SRC_FILES) 

link: 
	g++ $(OBJ_FILES) -L$(RD) -lRandomizer -Wl,-rpath,$(RD)	\
					 -o $(BD)argparsing

test:
	g++ -g -std=c++17 ../src/ArgParsing.cpp Test.cpp -o $(BD)argtest

clean:
	rm $(BD)argparsing $(BD)argtest *.o ../src/*.gch
