#include "Shell.h"
#include "Utility.h"

Shell::Shell() {
	this->running = true;
	this->cmdList = { 	new Command(true,"ls",lsFunc),
						new Command(true,"exit",quitFunc),
                //commands go here
    };
}

Command* Shell::get_command(const std::string& name) {
	for(int i=0;i<this->cmdList.size();i++) {
		if(cmdList.at(i)->name == name) {
			return this->cmdList.at(i);
		}
	}
	return nullptr;
}

//tokenize and run command function or execute
std::string Shell::parse_input(const std::string &user_input) {
    std::vector<std::string> tokenized = split(user_input,' ');
    std::string cmd = tokenized.at(0);

	Command* testCommand = this->get_command(cmd);

	if(testCommand != nullptr) {
		return testCommand->func(tokenized);
	}

    return cmd + ": command not found";
}
