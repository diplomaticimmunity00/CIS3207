#include <iostream>
#include <string>

#include "Common.h"

int main(int argc, char** argv) {

	std::string input;

	std::string ps = "myshell> ";

	while(myshell.running) {
		std::cout << ps << std::flush;
		std::getline(std::cin,input);
		std::cout << myshell.parse_input(input) << std::endl;
	}

	return 0;

}
