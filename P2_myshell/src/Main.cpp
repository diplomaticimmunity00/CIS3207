#include <iostream>
#include <string>

#include "Common.h"
#include "Utility.h"

int main(int argc, char** argv) {

	std::string input;

	while(myshell.running) {
		myshell.prompt_input(input);
		std::cout << myshell.parse_input(input) << std::flush;
	}

	return 0;

}
