myshell: bin/Utility.o bin/Main.o bin/Command.o bin/Common.o bin/Shell.o
	g++ -std=c++11 -fpermissive bin/Utility.o bin/Main.o bin/Command.o bin/Common.o bin/Shell.o -o myshell

bin/Utility.o: src/Utility.cpp src/Utility.h
	g++ -std=c++11 -fpermissive -c src/Utility.cpp -o bin/Utility.o

bin/Main.o: src/Main.cpp 
	g++ -std=c++11 -fpermissive -c src/Main.cpp -o bin/Main.o

bin/Command.o: src/Command.cpp src/Command.h
	g++ -std=c++11 -fpermissive -c src/Command.cpp -o bin/Command.o

bin/Common.o: src/Common.cpp src/Common.h
	g++ -std=c++11 -fpermissive -c src/Common.cpp -o bin/Common.o

bin/Shell.o: src/Shell.cpp src/Shell.h
	g++ -std=c++11 -fpermissive -c src/Shell.cpp -o bin/Shell.o

