#include "Shell.h"

#include "Utility.h"
#include "Common.h"

#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

Shell::Shell() {
	
	this->running = true;
	//initialize prompt string with current directory
	this->set_prompt_string();
	//set env var
	char *cwd = ("PWD="+convert(get_current_dir_name())).c_str();
	putenv(cwd);


	//list of internal commands
									//minargs,aliases,function
	this->cmdList = {	new Command(0,{"cd"},cdFunc),
						new Command(0,{"exit","quit"},quitFunc),
						new Command(0,{"help"},helpFunc),
						new Command(0,{"echo"},echoFunc),
						new Command(0,{"pwd"},pwdFunc),
						new Command(0,{"clear","clr"},clearFunc),
						new Command(0,{"pause"},pauseFunc),
						new Command(0,{"dir"},dirFunc),
						new Command(0,{"environ"},environFunc),
						new Command(0,{"STOP"},stopFunc),
						new Command(0,{"path"},pathFunc),
						
	};
}

//check if command is internal
//return command pointer or null pointer if command not found
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

//tokenize user input and run internal command or external executable
void Shell::parse_input(const std::string &user_input) {

	std::string cmdReturn;
	std::vector<std::string> tokenized = split(user_input,' ');
	std::string cmd;

	//target_command is used in input redirection to
	// run the same command with each subequent line of input
	if(target_command != "") {
		//if we are not processing input from a file
		if(tokenized.at(0) != "STOP") {
			//and EOF has not been reached
			// run command specified when input redirection was invoked
			cmd = target_command;
		} else {
			//otherwise assume command was provided traditionally
			// as the first token in user input
			cmd = tokenized.at(0);
		}
	} else {
		cmd = tokenized.at(0);
	}

	//strip whitespace from command
	cmd = strip(cmd);

	//ignore comments
	if(cmd[0] == '#') {
		return;
	}

	//check if command is internal
	Command* testCommand = this->find_command(cmd);

	//get index in argument vector of last redirect token/ampersand
	//previous tokens are ignored and considered arguments
	short input = rfind(tokenized,"<");
	short append = rfind(tokenized,">>");
	short truncate = rfind(tokenized,">");
	short pipe_index = rfind(tokenized,"|");
	short background_run = rfind(tokenized,"&");

	//choose which of each token type comes last in the argument vector
	int output_redir = (append > truncate)? append : truncate;
	int input_redir = (input > pipe_index)? input : pipe_index;

	//if an ampersand was provided with a command preceeding it
	if(background_run > 0) {
		//fork for each command and don't wait for exit
		int last_token_index = 0;
		for(int i=0;i<tokenized.size();i++) {
			if(tokenized.at(i) == "&") {
				int pid = fork();
				if(pid == 0) {
					//send command back to shell in a new process
					char* penv = ("PARENT="+this->shell).c_str();
					putenv(penv);
					this->parse_input(join(sub_vec(tokenized,last_token_index,i),' '));
					exit(0);
				} else if(pid > 0) {
					//and don't wait for processes to finish
					last_token_index = i+1;
				} else {
					//fork failed
					std::cerr << "An error has occurred." << std::endl;
				}
			}
		}
		//if the last command specified does not have an ampersand after it 
		// run it normally
		// otherwise it would have been handled in the previous loop
		if(tokenized.at(tokenized.size()-1) != "&") {
			this->parse_input(join(sub_vec(tokenized,background_run+1,tokenized.size()),' '));
		}
		return;
	}

	//If a pipe is used in the command
	if(pipe_index > 0) {
		int pipe_fds[2];

		if(pipe(pipe_fds) < 0) {
			//pipe failed
			std::cerr << "An error has occurred." << std::endl;
		}

		//read and write ends of new pipe
		int new_stdin = pipe_fds[0];
		int new_stdout = pipe_fds[1];

		int pid = fork();

		//first child writes to pipe
		if(pid == 0) {

			char* penv = ("PARENT="+this->shell).c_str();
			putenv(penv);

			//close stdin in child process
			close(new_stdin);
			dup2(new_stdout,1);

			//run the first command and send the output to the pipe, exit
			this->parse_input(join(sub_vec(tokenized,0,pipe_index),' '));
			exit(0);

		} else if(pid > 0) {
			//wait for pipe to fill up with output from child 1
			waitpid(pid,NULL,0);

		} else {
			//fork failed
			std::cerr << "An error has occurred." << std::endl;
		}

		int pid2 = fork();

		//second child reads from pipe and prints to stdout
		if(pid2 == 0) {
			char* penv = ("PARENT="+this->shell).c_str();
			putenv(penv);
			close(new_stdout);
			dup2(new_stdin,0);
			this->parse_input(join(sub_vec(tokenized,pipe_index+1,tokenized.size()),' '));
			exit(0);
		} else if(pid2 > 0) {
			close(new_stdin);close(new_stdout);
			waitpid(pid2,NULL,0);
		} else {
			std::cerr << "An error has occurred." << std::endl;
		
		}
		
		restore_stdout();
		return;
	}

	//If input is redirected
	// read inputs from source file 
	// until stop command
	if(input_redir > 0) {
		std::string fn = tokenized.at(input_redir+1);
		//verify that file has STOP command in it
		if(!verify(fn)) {
			std::cerr << "Error, file "+fn+" not formatted for myshell or does not exist" << std::endl;
			return;
		} else {
			reading_from_file = true;
			target_command = cmd;
			if(input_redir+1 >= tokenized.size()) {
				std::cerr <<  "An error has occurred" << std::endl;
				return;
			}
			char *filename = tokenized.at(input_redir+1).c_str();
			stdin_fd = dup(0);
			int new_stdin = open(filename,O_RDONLY,S_IRWXU);
			//std::cerr << "changed input to "+convert(filename) << std::endl;
			if(running_script) {
				//if we are running a batch file
				// clear the rest of the batch file
				// from the input buffer before redirecting
				// in from new file
				std::string f;
				while(f != "STOP") {
					std::getline(std::cin,f);
				}
			}
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
		if(output_redir+1 >= tokenized.size()) {
			std::cerr <<  "An error has occurred" << std::endl;
			return;
		}
		char *filename = tokenized.at(output_redir+1).c_str();
		
		//global int to keep track of current stdout file descriptor
		stdout_fd = dup(1);
		int new_stdout;
		//check if we are appending or truncating file
		if(output_redir == append) {
			new_stdout = open(filename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
		} else {
			new_stdout = open(filename,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
		}
		dup2(new_stdout,1);
		close(new_stdout);
	}

	//if input is redirected and was redirected
	// on this line, don't bother parsing the line
	if(reading_from_file and input_redir >= 0) {
		return;
	}

	//std::cerr << join(tokenized,' ');

	//remove command from tokenized arguments and
	//terminate arguments after redirect tokens
	int arg_finish = (output_redir < 0)? tokenized.size() : output_redir;
	tokenized = sub_vec(tokenized,(reading_from_file)? 0 : 1,arg_finish);

	//if command is internal
	if(testCommand != nullptr) {
		//get return from running command internally
		cmdReturn = testCommand->func(tokenized);
	} else {
		//otherwise check if executable exists in shell path
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
		//if executable is found
		if(!access(execname,X_OK)) {
			pid_t pid = fork();
			//fork and run
			if(pid == 0) {
				char* penv = ("PARENT="+this->shell).c_str();
				putenv(penv);
				//child
				//argv needs room for executable name and 0 termination
				char* argv[2+tokenized.size()];
				//convert c++ vector to char**
				populate(argv,tokenized);
				argv[0] = execname;
				int exerr = execvp(execname,argv);
				exit(exerr);
			} else if(pid > 0) {
				//parent
				//wait for child to terminate then return
				//Check if program was background executed
				if(background_run == -1) {
					waitpid(pid,NULL,0);
				}
			} else {
				//fork failed
				std::cerr <<  "An error has occurred" << std::endl;
			}
		} else {
			//if command is neither external nor internal 
			// then it doesn't exist 0_0
			cmdReturn = cmd + ": command not found\n";
		}
	}

	std::cout << cmdReturn << std::flush;

	//if we are not reading from a file but 
	// output was redirected try to restore stdout to terminal window
	if(!reading_from_file and output_redir > 0) {
		restore_stdout();
	}
}
