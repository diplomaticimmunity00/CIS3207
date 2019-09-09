DES: bin/Common.o bin/Main.o
	g++ bin/Common.o bin/Main.o -o DES

bin/Common.o: Common.cpp Common.h
	g++ -c Common.cpp -o bin/Common.o

bin/Main.o: Main.cpp
	g++ -c Main.cpp -o bin/Main.o
