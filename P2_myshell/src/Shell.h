#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "Command.h"

std::string parse_input(const std::string&);

struct Shell {

	bool running;
	std::vector<Command*> cmdList;

	std::string parse_input(const std::string&);

	//return command by name or nullptr
	Command* get_command(const std::string&);

	Shell();	

};
