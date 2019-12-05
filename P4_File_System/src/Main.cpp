#include <iostream>
#include <string>
#include "Filesys.h"
#include "Utility.h"
#include "Common.h"
#include "disk.h"

int main(int argc, char** argv) {

	
	
	//create disk
	if(!file_exists("vsda")) {
		print("=================");
		print("   CREATE DISK"	 );
		print("=================");
		make_fs("vsda");
	}

	print("=================");
	print("   MOUNT DISK"	 );
	print("=================");

	//read inode table from disk
	mount_fs("vsda");

	print("=================");
	print("  CREATE FILES	");
	print("=================");

	//create 3 new files and add them to the inode table
	fs_create("/test1.txt");
	fs_create("/test2.txt");
	fs_create("/test3.txt");

	fs_mkdir("/home");

	fs_create("/home/notes.txt");

	fs_mkdir("/home/test3");

	fs_create("/home/test3/bigfile.txt");

	print("Total files in / directory: "+std::to_string(inode_table.at(get_file_index("/"))->children));
	print("Total files in /home directory: "+std::to_string(inode_table.at(get_file_index("/home"))->children));

	print("=================");
	print("     FILE I/O"    );
	print("=================");

	int fd1 = fs_open("/test1.txt");
	int fd2 = fs_open("/home/test3/bigfile.txt");

	print("=================");
	print("     WRITING"    );
	print("=================");

	// write string to files
	char buffer1[128] = "cooltest";
	// fs_write(fd1,buffer1,128);
	// fs_write(fd1,buffer1,128);
	fs_write(fd1,buffer1,128);

	print("=================");
	print("     READING"    );
	print("=================");

	char contents[BLOCK_SIZE*8]	;
	clear_buffer(contents,BLOCK_SIZE*8);
	fs_read(fd1,contents,BLOCK_SIZE*8);
	Inode *testfile = inode_table.at(get_file_index_by_fd(fd1));
	print("File "+testfile->name+" contents after writing: "+contents);

	print("=================");
	print("   TRUNCATING"    );
	print("=================");
	print("Truncating file "+testfile->name+" to 3 bytes");
	fs_truncate(fd1,3);

	clear_buffer(contents,BLOCK_SIZE*8);
	fs_read(fd1,contents,BLOCK_SIZE*8);
	print("File "+testfile->name+" contents after writing: "+contents);
	print("Real size on disk for file "+testfile->name+": "+std::to_string(testfile->size)+" bytes");
	
	print("===================");
	print("INTER-BLOCK WRITING"    );
	print("===================");

	char dummy[BLOCK_SIZE*400] = "1111111";
	
	for(int i=0;i<6;i++) {
		sprintf(dummy,"%s%s%s",dummy,dummy,dummy);
	}
	fs_write(fd2,dummy,BLOCK_SIZE*400);

	print("=================");
	print("       SIZE"    );
	print("=================");

	Inode *testfile2 = inode_table.at(get_file_index_by_fd(fd2));
	print("File "+testfile2->name+" size after writing: "+std::to_string(testfile2->size));

	print("=================");
	print("  CLOSE FILES"    );
	print("=================");

	//delete file contents and inode
	fs_close(fd1);
	fs_close(fd2);

	print("=================");
	print("  DELETE FILES"   );
	print("=================");

	fs_delete("/home/test3/bigfile.txt");

	print("=================");
	print("  UNMOUNT DISK"   );
	print("=================");

	//write new inode table to disk
	umount_fs("vsda");

	return 0;

}
