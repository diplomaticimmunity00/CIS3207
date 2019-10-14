#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Common.h"
#include "Utility.h"

int main(int argc, char** argv) {

	std::string input;

	//if a batch script was provided
	if(argc > 1) {
		std::string fn = convert(argv[1]);
		if(!file_exists(fn)) {
			std::cerr << "File "+fn+" not found" << std::endl;
			exit(1);
		}
		if(!verify(convert(argv[1]))) {
			std::cerr << "Error: file "+fn+" not formatted for myshell" << std::endl;
			exit(1);
		}

		//if file exists and is properly formatted
		// redirect input to specified file
		running_script = true;
		filename = argv[1];
		stdin_fd = dup(0);
		int new_fd = open(filename,O_RDONLY);
		dup2(new_fd,0);
		close(new_fd);
	}

	while(myshell.running) {
							//don't print prompt string if reading from file
		myshell.prompt_input(input,reading_from_file | running_script);
		myshell.parse_input(input);
		//for keeping track of place in batch file
		if(running_script and !reading_from_file) script_line++;
	}
	return 0;

}
