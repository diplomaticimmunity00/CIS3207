#include <iostream>
#include <string>

int main(int argc, char** argv) {

	std::string input;

	std::cout << argv[1] << std::endl;

	while(1) {
		std::cin >> input;
		std::cout << input << std::endl;
	}

	return 1;

}
