#include "Shell.h"
#include "Utility.h"

Shell::Shell() {
	this->running = true;
							//minArgs,name,function
	this->cmdList = { 	new Command(0,{"cd"},cdFunc),
						new Command(0,{"exit","quit"},quitFunc),
						new Command(0,{"echo"},echoFunc),
						new Command(0,{"pwd"},pwdFunc),
						new Command(1,{"cat"},catFunc),
						new Command(0,{"clear"},clearFunc),
						
    };
}

Command* Shell::find_command(const std::string& name) {
	for(int i=0;i<this->cmdList.size();i++) {
		for(int j=0;j<cmdList.at(i)->aliases.size();j++) {
			if(cmdList.at(i)->aliases.at(j) == name) {
				return this->cmdList.at(i);
			}
		}
	}
	return nullptr;
}

//tokenize and run command function or execute
std::string Shell::parse_input(const std::string &user_input) {
    std::vector<std::string> tokenized = split(user_input,' ');
    std::string cmd = tokenized.at(0);

	//remove command from tokenized arguments
	tokenized = sub_vec(tokenized,1,tokenized.size());

	Command* testCommand = this->find_command(cmd);

	if(testCommand != nullptr) {
		if(tokenized.size() < testCommand->minArgs) {
			//user has not supplied enough arguments
			//error message should be command specific
			return "CMD_ARG_ERROR_STR\n";
		}
		return testCommand->func(tokenized);
	}

    return cmd + ": command not found\n";
}
