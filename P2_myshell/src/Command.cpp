#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Command.h"
#include "Utility.h"
#include "Common.h"


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
		return testPath + ": error opening directory\n";
	}

	chdir(testPath.c_str());
	myshell.set_prompt_string();
	return "";

}

std::string clearFunc(const std::vector<std::string> &args) {
	print("\033[H\033[2J");	
	return "";
}

std::string helpFunc(const std::vector<std::string> &args) {
    return "PLACEHOLDER\n";
}

std::string echoFunc(const std::vector<std::string> &args) {
	return join(args,' ')+"\n";
}

std::string quitFunc(const std::vector<std::string> &args) {
	myshell.running = false;
    return "";
}

std::string pauseFunc(const std::vector<std::string> &args) {
	print("Press enter to continue...");
	std::string x;
	std::getline(std::cin,x);
	return "";
}

std::string dirFunc(const std::vector<std::string> &args) {
	DIR *d;
	if(args.size() == 0) {
		d = opendir(get_current_dir_name());
	}
	else {
		d = opendir(args.at(0).c_str());
	}
	if(!d) {
		return args.at(0) + ": error opening directory\n";
	}
	dirent *file;
	std::string contents;
	while( (file = readdir(d)) != NULL) {
		if(file->d_name[0] == '.') continue;
		contents += convert(file->d_name) + ' ';
	}
	return contents + "\n";
}

std::string environFunc(const std::vector<std::string> &args) {
	std::vector<std::string> envVars = {"USER"};
	std::string envString = "";
	for(int i=0;i<envVars.size();i++) {
		envString += envVars.at(i) + ": " + convert(getenv(envVars.at(i).c_str())) + "\n";
	}
	envString += "PATH: "+join(myshell.paths,':') + '\n';
	return envString;
}

std::string stopFunc(const std::vector<std::string> &args) {
	if(reading_from_file) {
		dup2(cin_fd,0);
		close(cin_fd);
		reading_from_file = false;
		if(running_script) exit(0);
		target_command = "";
	}
	int test_stdout = dup(1);
	if(stdout_fd != test_stdout) {
    	dup2(stdout_fd,1);
	}
	close(test_stdout);
	return "";
}
std::string pathFunc(const std::vector<std::string> &args) {
	myshell.paths = args;
	if(args.size() == 0) {
		return "Unset path\n";
	}
	return "Set path = "+join(args,':') + "\n";
}

