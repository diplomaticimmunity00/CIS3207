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
	std::string wd = convert(get_current_dir_name());
	if(wd[0] != '/') wd = "/"+wd;
	return wd + "\n";
}

std::string cdFunc(const std::vector<std::string> &args) {
	if(args.size() == 0) {
		chdir(myshell.root.c_str());
		return "";
	}

	std::string testPath = args.at(0);
	
	DIR *d = opendir(testPath.c_str());

	if(!d) {
		return testPath + ": directory not found\n";
	}

	chdir(testPath.c_str());
	return "";

}

std::string clearFunc(const std::vector<std::string> &args) {
	print("\033[H\033[2J");	
	return "";
}

std::string echoFunc(const std::vector<std::string> &args) {
	return join(args,' ')+"\n";
}

std::string quitFunc(const std::vector<std::string> &args) {
	myshell.running = false;
    return "Exiting\n";
}

std::string pauseFunc(const std::vector<std::string>&) {
	print("Press enter to continue...");
	std::string x;
	std::getline(std::cin,x);
	return "";
}

std::string dirFunc(const std::vector<std::string>&) {
	return "PLACEHOLDER\n";
}

std::string environFunc(const std::vector<std::string>&) {
	return getenv("USER");
}
