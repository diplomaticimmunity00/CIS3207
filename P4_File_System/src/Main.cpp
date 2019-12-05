#include <iostream>
#include <string>
#include "Filesys.h"
#include "Utility.h"
#include "Common.h"
#include "disk.h"

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

	int fd1 = fs_open("test1.txt");

	// write string to files
	char buffer1[128] = "cooltest";
	fs_write(fd1,buffer1,128);
	fs_write(fd1,buffer1,128);
	fs_write(fd1,buffer1,128);

	fs_truncate(fd1,5);

	// for(int i=0;i<100;i++) {
		// for(int j=0;j<100;j++) {
			// buffer1[j] = (char)i;
		// }
		// fs_write(fd1,buffer1,BLOCK_SIZE*2);
	// }

	char contents[BLOCK_SIZE*5];
	clear_buffer(contents,BLOCK_SIZE*5);
	fs_read(fd1,contents,BLOCK_SIZE*5);
	print(contents);
	// sprintf(buffer1,"%s%s%s",buffer1,buffer1,buffer1);
	
	//delete file contents and inode
	fs_close(fd1);

	//exit(0);

	// fs_delete("test3.txt");

	//write new inode table to disk
	umount_fs("vsda");

	return 0;

}
