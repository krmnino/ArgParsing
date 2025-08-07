RD = "../res/"

all:
	g++ -c -fPIC ArgParsing.cpp -o $(RD)ArgParsing.o
	g++ -shared -o $(RD)libArgParsing.so $(RD)ArgParsing.o

debug:
	g++ -c -D DEBUG -fPIC ArgParsing.cpp -o $(RD)ArgParsing.o
	g++ -shared -o $(RD)libArgParsing.so $(RD)ArgParsing.o

clean:
	rm $(RD)*.o $(RD)*.so