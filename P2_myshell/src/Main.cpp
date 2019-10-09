#include <iostream>
#include <string>

#include "Common.h"
#include "Utility.h"

int main(int argc, char** argv) {

	std::string input;

	while(myshell.running) {
		myshell.prompt_input(input);
		myshell.parse_input(input);
	}

	return 0;

}
