#include <iostream>
#include <fstream>

int main(int argc,char** argv) {
    std::ifstream f(argv[1]);
    std::string line;

    if(!f.is_open()) {
		std::cout << argv[1] <<  ": error opening file\n" << std::flush;
		return 1;
    }

    while(std::getline(f,line)) {
		std::cout << line << "\n";
    }

	std::cout << std::flush;
    return 0;
}
