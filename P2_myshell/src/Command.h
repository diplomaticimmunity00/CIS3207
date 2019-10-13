#pragma once

#include <iostream>
#include <vector>
#include <string>

//command funcs

//cd
std::string cdFunc(const std::vector<std::string>&);
//dir
std::string dirFunc(const std::vector<std::string>&);
//exit
std::string quitFunc(const std::vector<std::string>&);
//echo
std::string echoFunc(const std::vector<std::string>&);
//pwd
std::string pwdFunc(const std::vector<std::string>&);
//clear
std::string clearFunc(const std::vector<std::string>&);
//cat
//std::string catFunc(const std::vector<std::string>&);
//pause
std::string pauseFunc(const std::vector<std::string>&);
//environ
std::string environFunc(const std::vector<std::string>&);
//help
std::string helpFunc(const std::vector<std::string>&);
//restore standard input
std::string stopFunc(const std::vector<std::string>&);
//set path
std::string pathFunc(const std::vector<std::string>&);

struct Command {

	//whether or not command is internal to shell or an external executable
	int minArgs;
	const std::vector<std::string> aliases;
	
	//call function for internal commands
	std::string (*func)(const std::vector<std::string>&);
	//executable filename for external commands
	const std::string executable;

	Command();
	Command(int _minArgs,const std::vector<std::string>& _aliases, std::string (*_func)(const std::vector<std::string>&)): minArgs(_minArgs), aliases(_aliases), func(_func) {}

};
