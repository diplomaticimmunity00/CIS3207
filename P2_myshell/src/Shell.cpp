#include "Shell.h"
#include "Utility.h"

#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

Shell::Shell() {
	this->running = true;
							//minArgs,name,function
	this->cmdList = {	new Command(0,{"cd"},cdFunc),
						new Command(0,{"exit","quit"},quitFunc),
						new Command(0,{"help"},helpFunc),
						new Command(0,{"echo"},echoFunc),
						new Command(0,{"pwd"},pwdFunc),
						//new Command(1,{"cat"},catFunc),
						new Command(0,{"clear"},clearFunc),
						new Command(0,{"pause"},pauseFunc),
						new Command(0,{"dir"},dirFunc),
						new Command(0,{"environ"},environFunc),
						new Command(0,{"STOP"},stopFunc),
						
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
void Shell::parse_input(const std::string &user_input) {
	std::string cmdReturn;
	std::vector<std::string> tokenized = split(user_input,' ');
	std::string cmd = tokenized.at(0);

	Command* testCommand = this->find_command(cmd);

	int old_fd,new_fd;
	
	//get index in argument vector of last redirect token
	//subsequent tokens are ignored
	short append = rfind(tokenized,">>");
	short create = rfind(tokenized,">");
	//choose which token comes last in the argument vector
	//	treat everything between the command and that token as arguments
	int output_redir = (append > create)? append : create;

	//If output is redirected
	//	duplicate stdout
	//	replace with output file fd in process fd table
	//	close duplicate file
	//	replace stdout after command finishes
	if(output_redir >= 0) {
		old_fd = dup(1);
		if(output_redir == append) {
			new_fd = open(tokenized.at(output_redir+1).c_str(),O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
		} else {
			new_fd = open(tokenized.at(output_redir+1).c_str(),O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
		}

		dup2(new_fd,1);
		close(new_fd);
	}

	int arg_finish = (output_redir < 0)? tokenized.size() : output_redir;

	//remove command from tokenized arguments and
	//terminate arguments after redirect token
	tokenized = sub_vec(tokenized,1,arg_finish);

	if(testCommand != nullptr) {
		if(tokenized.size() < testCommand->minArgs) {
			//user has not supplied enough arguments
			//error message should be command specific
			cmdReturn = "CMD_ARG_ERROR_STR\n";
		} else {
			cmdReturn = testCommand->func(tokenized);
		}
	} else {
		//if executable exists in shell path
		char* execname;
		execname = cmd.c_str();
		if(access(execname,X_OK)) {
	    	for(int i=0;i<this->paths.size();i++) {
				execname = (this->paths.at(i)+cmd).c_str();
				if(!access(execname,X_OK)) {
					break;
				}
			}
		}
		if(!access(execname,X_OK)) {
			pid_t pid = fork();
			if(pid == 0) {
				//child
				char* argv[2+tokenized.size()];
            	populate(argv,tokenized);
            	argv[0] = execname;
				int exerr = execvp(execname,argv);
				exit(0);
			} else if(pid > 0) {
				//parent
				//wait for child to terminate then return
				int status;
				waitpid(pid,&status,0);
			} else {
				std::cerr <<  "An error has occurred" << std::endl;
			}
		} else {
			cmdReturn = cmd + ": command not found\n";
		}
	}

	std::cout << cmdReturn << std::flush;

	if(output_redir >= 0) {
		dup2(old_fd,1);
		close(old_fd);
	}

}
