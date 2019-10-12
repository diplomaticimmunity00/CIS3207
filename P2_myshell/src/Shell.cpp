#include "Shell.h"

#include "Utility.h"
#include "Common.h"

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
	
	std::string cmd;

	if(target_command != "") {
		if(tokenized.at(0) != "STOP") {
			cmd = target_command;
		} else {
			cmd = tokenized.at(0);
		}
	} else {
		cmd = tokenized.at(0);
	}


	Command* testCommand = this->find_command(cmd);

	//get index in argument vector of last redirect token
	//subsequent tokens are ignored
	short input = rfind(tokenized,"<");
	short append = rfind(tokenized,">>");
	short truncate = rfind(tokenized,">");
	short pipe = rfind(tokenized,"|");

	//choose which token comes last in the argument vector
	//	treat everything between the command and that token as arguments
	int output_redir = (append > truncate)? append : truncate;
	int input_redir = (input > pipe)? input : pipe;

	//If input is redirected
	// read inputs from source file 
	// until stop command
	if(input_redir > 0) {
		reading_from_file = true;
		target_command = cmd;
        char *filename = tokenized.at(input_redir+1).c_str();
        cin_fd = dup(0);
        int new_stdin = open(filename,O_RDONLY);
        dup2(new_stdin,0);
        close(new_stdin);
	}

	//If output is redirected
	//	duplicate stdout
	//	replace with output file fd in process fd table
	//	close duplicate file
	//	replace stdout after command finishes
	if(output_redir > 0) {
		stdout_fd = dup(1);
		int new_stdout;
		if(output_redir == append) {
			new_stdout = open(tokenized.at(output_redir+1).c_str(),O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
		} else {
			new_stdout = open(tokenized.at(output_redir+1).c_str(),O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
		}

		dup2(new_stdout,1);
		close(new_stdout);
	}

	if(input_redir >= 0) return;

	//remove command from tokenized arguments and
	//terminate arguments after redirect token
	int arg_finish = (output_redir < 0)? tokenized.size() : output_redir;
	tokenized = sub_vec(tokenized,(reading_from_file != running_script)? 0 : 1,arg_finish);

	if(testCommand != nullptr) {
		if(tokenized.size() < testCommand->minArgs) {
			//user has not supplied enough arguments
			//error message should be command specific
			cmdReturn = "CMD_ARG_ERROR_STR\n";
		} else {
			cmdReturn = testCommand->func(tokenized);
		}
	} else {
		//check if executable exists in shell path
		// or if specified path was absolute
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
				exit(exerr);
			} else if(pid > 0) {
				//parent
				//wait for child to terminate then return
				int status;
				//Check if program was background executed here
				waitpid(pid,&status,0);
			} else {
				std::cerr <<  "An error has occurred" << std::endl;
			}
		} else {
			cmdReturn = cmd + ": command not found\n";
		}
	}

	std::cout << cmdReturn << std::flush;

	if(!reading_from_file and output_redir >= 0) {
		dup2(stdout_fd,1);
		close(stdout_fd);
	}

}
