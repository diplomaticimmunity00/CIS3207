#pragma once

#include <iostream>
#include <vector>
#include <string>

//command funcs

//cd
std::string cdFunc(const std::vector<std::string>&);
//exit
std::string quitFunc(const std::vector<std::string>&);
//echo
std::string echoFunc(const std::vector<std::string>&);
//pwd
std::string pwdFunc(const std::vector<std::string>&);
//cat
std::string catFunc(const std::vector<std::string>&);



struct Command {

	//whether or not command is internal to shell or an external executable
	int minArgs;
	const std::string name;
	
	//call function for internal commands
	std::string (*func)(const std::vector<std::string>&);
	//executable filename for external commands
	const std::string executable;

	Command();
	Command(int _minArgs,const std::string& _name, std::string (*_func)(const std::vector<std::string>&)): minArgs(_minArgs), name(_name), func(_func) {}

};
