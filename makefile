DES: bin/Config.o bin/Component.o bin/Main.o bin/Clock.o bin/Event.o bin/Simulation.o bin/Common.o
	g++ -std=c++11 bin/Config.o bin/Component.o bin/Main.o bin/Clock.o bin/Event.o bin/Simulation.o bin/Common.o -o DES

bin/Config.o: src/Config.cpp src/Config.h
	g++ -std=c++11 -c src/Config.cpp -o bin/Config.o

bin/Component.o: src/Component.cpp src/Component.h
	g++ -std=c++11 -c src/Component.cpp -o bin/Component.o

bin/Main.o: src/Main.cpp 
	g++ -std=c++11 -c src/Main.cpp -o bin/Main.o

bin/Clock.o: src/Clock.cpp src/Clock.h
	g++ -std=c++11 -c src/Clock.cpp -o bin/Clock.o

bin/Event.o: src/Event.cpp src/Event.h
	g++ -std=c++11 -c src/Event.cpp -o bin/Event.o

bin/Simulation.o: src/Simulation.cpp src/Simulation.h
	g++ -std=c++11 -c src/Simulation.cpp -o bin/Simulation.o

bin/Common.o: src/Common.cpp src/Common.h
	g++ -std=c++11 -c src/Common.cpp -o bin/Common.o

