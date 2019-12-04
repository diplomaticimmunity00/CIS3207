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

	create_file("test1.txt");
	create_file("test2.txt");
	create_file("test3.txt");

	print(fs_open("test3.txt"));

	//write new inode table to disk
	umount_fs("vsda");

	return 0;

}
