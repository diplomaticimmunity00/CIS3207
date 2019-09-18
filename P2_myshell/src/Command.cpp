#include <iostream>

#include "Command.h"
#include "Utility.h"

std::string lsFunc(const std::vector<std::string> &args) {
	return "DIRECTOYR!!";
}

std::string quitFunc(const std::vector<std::string> &args) {
	std::system("./hi");
	exit(0);
    return "Exiting";
}
