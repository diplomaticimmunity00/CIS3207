#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "Command.h"

std::string parse_input(const std::string&);

struct Shell {

	bool running;
	std::vector<Command*> cmdList;

	//Environment variables
	std::string ps = "myshell> ";

	std::string root = "/";
	std::string cwd = "/";

	void prompt_input(std::string &input) {
		std::cout << this->ps << std::flush;
		std::getline(std::cin,input);
	}

	std::string parse_input(const std::string&);

	//return command by name or nullptr
	Command* find_command(const std::string&);

	Shell();	

};
