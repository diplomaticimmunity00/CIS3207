#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Command.h"
#include "Utility.h"
#include "Common.h"

std::string catFunc(const std::vector<std::string> &args) {
	pid_t pid = fork();
	if(pid == 0) {
		//child
		char *fn = args.at(0).c_str();
		char* argv[3] = {"ls",fn,0};
		char* env[1] = {0};
		int exerr = execve("exec/cat",argv,env);
		exit(0);
	} else if(pid > 0) {
		//parent
		int status;
		wait(&status);
		return "";
	} else {
		return "Fork failed";
	}
}

std::string pwdFunc(const std::vector<std::string> &args) {
	return myshell.cwd+"\n";
}

std::string cdFunc(const std::vector<std::string> &args) {
	if(args.size() == 0) {
		myshell.cwd = myshell.root;
		return "";
	}

	std::string testPath = args.at(0);
	
	//check relative path
	DIR *d = opendir((myshell.cwd + "/" + testPath).c_str());

	if(!d or testPath[0] == '/') {
		//otherwise check absolute path
		DIR *dabs = opendir(testPath.c_str());
		if(!dabs) {
			return testPath + ": directory not found\n";
		} else {
			myshell.cwd = testPath;
		}
	} else {
		//path specified was relative
		myshell.cwd += "/" + testPath;
	}

	return "";
}

std::string echoFunc(const std::vector<std::string> &args) {
	return join(sub_vec(args,1,args.size()),' ')+"\n";
}

std::string quitFunc(const std::vector<std::string> &args) {
	myshell.running = false;
    return "Exiting\n";
}
