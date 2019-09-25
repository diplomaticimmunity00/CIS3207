#include <iostream>
#include <string>
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
		char* argv[3] = {"",fn,0};
		int exerr = execvp("exec/cat",argv);
		exit(0);
	} else if(pid > 0) {
		//parent
		//wait for child to terminate then return
		int status;
		waitpid(pid,&status,0);
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

std::string clearFunc(const std::vector<std::string> &args) {
	for(int i=0;i<50;i++) {
		print("\n");
	}
	return "";
}

std::string echoFunc(const std::vector<std::string> &args) {
	return join(args,' ')+"\n";
}

std::string quitFunc(const std::vector<std::string> &args) {
	myshell.running = false;
    return "Exiting\n";
}
