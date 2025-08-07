RD = ../res/
BD = ../bin/

SRC_FILES = ArgTableBuilder.cpp		\
			Main.cpp

OBJ_FILES = ArgTableBuilder.o		\
			Main.o

all: compile link

compile:
	g++ -g -c -D DEBUG -std=c++17 $(SRC_FILES) 

link: 
	g++ $(OBJ_FILES) -L$(RD) -lRandomizer -Wl,-rpath,$(RD)	\
					 -L$(RD) -lArgParsing -Wl,-rpath,$(RD)	\
					 -o $(BD)argparsing

test:
	g++ -g -std=c++17 -D DEBUG ../src/ArgParsing.cpp Test.cpp -o $(BD)argtest

clean:
	rm $(BD)argparsing $(BD)argtest *.o ../src/*.gch
