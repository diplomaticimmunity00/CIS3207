#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Common.h"
#include "Utility.h"

int main(int argc, char** argv) {

	std::string input;

	if(argc > 1) {
		if(!file_exists(convert(argv[1]))) {
			exit(1);
		}
		reading_from_file = true;
		running_script = true;
		char *filename = argv[1];
		cin_fd = dup(0);
		int new_fd = open(filename,O_RDONLY);
		dup2(new_fd,0);
		close(new_fd);
	}

	while(myshell.running) {
		myshell.prompt_input(input,reading_from_file);
		myshell.parse_input(input);
	}
	return 0;

}
