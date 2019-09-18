#pragma once

#include <iostream>
#include <vector>
#include <string>

//command funcs

//ls
std::string lsFunc(const std::vector<std::string>&);
std::string quitFunc(const std::vector<std::string>&);

struct Command {

	//whether or not command is internal to shell or an external executable
	bool internal;
	const std::string name;
	
	//call function for internal commands
	std::string (*func)(const std::vector<std::string>&);
	//executable filename for external commands
	const std::string executable;

	Command();
	Command(bool _internal,const std::string& _name, std::string (*_func)(const std::vector<std::string>&)): internal(_internal), name(_name), func(_func) {}

};
