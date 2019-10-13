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
	this->set_prompt_string();
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
						new Command(0,{"path"},pathFunc),
						
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

	//strip whitespace from command;
	cmd = strip(cmd);

	Command* testCommand = this->find_command(cmd);

	//get index in argument vector of last redirect token
	//subsequent tokens are ignored
	short input = rfind(tokenized,"<");
	short append = rfind(tokenized,">>");
	short truncate = rfind(tokenized,">");
	short pipe_index = rfind(tokenized,"|");
	short background_run = rfind(tokenized,"&");

	//choose which token comes last in the argument vector
	//	treat everything between the command and that token as arguments
	int output_redir = (append > truncate)? append : truncate;
	int input_redir = (input > pipe_index)? input : pipe_index;

	if(background_run > 0) {
		int last_token_index = 0;
		for(int i=0;i<tokenized.size();i++) {
			if(tokenized.at(i) == "&") {
				int pid = fork();
				if(pid == 0) {
					this->parse_input(join(sub_vec(tokenized,last_token_index,i),' '));
					exit(0);
				} else if(pid > 0) {
					//don't wait for processes to finish
					last_token_index = i+1;
				} else {
					std::cerr << "Fork failed" << std::endl;
				}
			}
		}
		if(tokenized.at(tokenized.size()-1) != "&") {
			int pid = fork();
			if(pid == 0) {
				this->parse_input(join(sub_vec(tokenized,background_run+1,tokenized.size()),' '));
				exit(0);
			} else if(pid > 0) {
				//don't wait for processes to finish
			} else {
				std::cerr << "Fork failed" << std::endl;
			}
		}
		return;
	}

	//If a pipe is used in the command
	if(pipe_index > 0) {
		int pipe_fds[2];
		if(pipe(pipe_fds) < 0) {
			std::cerr << "Pipe failed" << std::endl;
		}
		int new_stdin = pipe_fds[0];
		int new_stdout = pipe_fds[1];

		int pid = fork();

		if(pid == 0) {
			close(new_stdin);
			dup2(new_stdout,1);
			//run the first command and send the output to the pipe, exit
			this->parse_input(join(sub_vec(tokenized,0,pipe_index),' '));
			exit(0);
		} else if(pid > 0) {
			//wait for pipe to fill up with output from child 1
			waitpid(pid,NULL,0);
		} else {
			std::cerr << "Pipe failed" << std::endl;
		}

		int pid2 = fork();

		if(pid2 == 0) {
			close(new_stdout);
			dup2(new_stdin,0);
			this->parse_input(join(sub_vec(tokenized,pipe_index+1,tokenized.size()),' '));
			exit(0);
		} else if(pid2 > 0) {
			close(new_stdin);close(new_stdout);
			waitpid(pid2,NULL,0);
		} else {
			std::cerr << "Pipe failed" << std::endl;
		
		}
		
		restore_stdout();
		return;
	}

	//If input is redirected
	// read inputs from source file 
	// until stop command
	if(input_redir > 0) {
		std::string fn = tokenized.at(input_redir+1);
		if(!verify(fn)) {
			cmd = "echo";
			tokenized = {"echo","Error, file "+fn+" not formatted for myshell or doesn't exist"};
			input_redir = -1;
		} else {
			reading_from_file = true;
			target_command = cmd;
			char *filename = tokenized.at(input_redir+1).c_str();
			cin_fd = dup(0);
			int new_stdin = open(filename,O_RDONLY);
			dup2(new_stdin,0);
			close(new_stdin);
		}
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

	if(reading_from_file and input_redir >= 0) return;

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
				if(background_run == -1) {
					waitpid(pid,&status,0);
				}
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
