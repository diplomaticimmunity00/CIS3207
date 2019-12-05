fs: bin/Utility.o bin/Filesys.o bin/Main.o bin/Inode.o bin/Common.o bin/disk.o
	g++ -std=c++11 -fpermissive bin/Utility.o bin/Filesys.o bin/Main.o bin/Inode.o bin/Common.o bin/disk.o -lpthread -o fs

bin/Utility.o: src/Utility.cpp src/Utility.h
	g++ -fpermissive -std=c++11 -c src/Utility.cpp -lpthread -o bin/Utility.o

bin/Filesys.o: src/Filesys.cpp src/Filesys.h
	g++ -fpermissive -std=c++11 -c src/Filesys.cpp -lpthread -o bin/Filesys.o

bin/Main.o: src/Main.cpp 
	g++ -fpermissive -std=c++11 -c src/Main.cpp -lpthread -o bin/Main.o

bin/Inode.o: src/Inode.cpp src/Inode.h
	g++ -fpermissive -std=c++11 -c src/Inode.cpp -lpthread -o bin/Inode.o

bin/Common.o: src/Common.cpp src/Common.h
	g++ -fpermissive -std=c++11 -c src/Common.cpp -lpthread -o bin/Common.o

bin/disk.o: src/disk.cpp src/disk.h
	g++ -fpermissive -std=c++11 -c src/disk.cpp -lpthread -o bin/disk.o

