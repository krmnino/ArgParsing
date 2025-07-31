SRC_FILES = ../src/ArgParsing.cpp	\
			Main.cpp

OBJ_FILES = ArgParsing.o			\
			Main.o

all: compile link

compile:
	g++ -g -c -std=c++17 $(SRC_FILES) 

link: 
	g++ -o argparsing $(OBJ_FILES)

test:
	g++ -g -std=c++17 -D DEBUG -o argtest ../src/ArgParsing.cpp Test.cpp

clean:
	rm argparsing argtest *.o ../src/*.gch
