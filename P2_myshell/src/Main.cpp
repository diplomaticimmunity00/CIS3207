#include <iostream>
#include <string>

#include "Common.h"

int main(int argc, char** argv) {

	std::string input;

	while(myshell.running) {
		std::cout << "myshell >" << std::flush;
		std::getline(std::cin,input);
		std::cout << myshell.parse_input(input) << std::endl;
	}

	return 1;

}
