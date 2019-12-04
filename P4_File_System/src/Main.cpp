#include <iostream>
#include <string>
#include "Filesys.h"
#include "Utility.h"
#include "Common.h"

int main(int argc, char** argv) {

	
	//create disk
	if(!file_exists("vsda")) {
		make_fs("vsda");
	}

	//read inode table from disk
	mount_fs("vsda");

	//create 3 new files and add them to the inode table
	fs_create("test1.txt");
	fs_create("test2.txt");
	fs_create("test3.txt");

	int fd = fs_open("test3.txt");

	// write string to test3.txt
	char buffer[32] = "TESTTESTTSETTSETTSETTESTSET";
	fs_write(fd,buffer,32);

	//read string from test3.txt
	char contents[32];
	clear_buffer(contents,32);
	fs_read(fd,contents,32);

	print(contents);

	//write new inode table to disk
	umount_fs("vsda");

	return 0;

}
