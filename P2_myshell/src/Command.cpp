#include <iostream>
#include <dirent.h>

#include "Command.h"
#include "Utility.h"
#include "Common.h"

std::string pwdFunc(const std::vector<std::string> &args) {
	return myshell.cwd+"\n";
}

std::string cdFunc(const std::vector<std::string> &args) {
	if(args.size() == 0) {
		myshell.cwd = myshell.root;
		return "";
	}

	std::string testPath = args.at(0);
	
	//try searching directories in cwd
	DIR *d = opendir((myshell.cwd + testPath).c_str());

	if(!d) {
		//otherwise check absolute path
		DIR *dabs = opendir(testPath.c_str());
		if(!dabs) {
			return testPath + ": directory not found\n";
		} else {
			myshell.cwd = testPath;
		}
	} else {
		//path specified was relative
		myshell.cwd += testPath;
	}

	return "";
}

std::string echoFunc(const std::vector<std::string> &args) {
	return join(sub_vec(args,1,args.size()),' ')+"\n";
}

std::string quitFunc(const std::vector<std::string> &args) {
	//std::system("./exec/hi");
	myshell.running = false;
    return "Exiting\n";
}
