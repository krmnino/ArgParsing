RD = "../res/"

all:
	g++ -c -fPIC -Wall -std=c++17 ArgParsing.cpp -o $(RD)ArgParsing.o
	g++ -shared -o $(RD)libArgParsing.so $(RD)ArgParsing.o

clean:
	rm $(RD)*.o $(RD)*.so