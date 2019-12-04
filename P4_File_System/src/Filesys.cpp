#include "Filesys.h"
#include "Utility.h"
#include "Common.h"
#include "Inode.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "Common.h"
#include "disk.h"

void init_disk_junk() {
	char buffer[BLOCK_SIZE];
	for(int i=0;i<DISK_BLOCKS;i++) {
		block_write(i,buffer);
	}
}

int init_inode_table() {
	//initializes first FILE_MAX blocks on the virtual disk with space for inodes
	for(int block=0;block<FILE_MAX;block++) {
		Inode* inode = new Inode();
		inode->id = block+1;
		inode->blocks_allocated = 1;
		inode->isFile = 1;
		inode->isValid = -1;
		inode->name = "DIRENT_NAME_UNDEFINED";
		inode_table.push_back(inode);
	}
	return 0;
}

int write_inode_table_to_vdisk() {
	char buffer[BLOCK_SIZE];
	int fd = open("/dev/zero", O_RDONLY);
	read(fd, buffer, BLOCK_SIZE);
	Inode* inode;
	print("writing inode data to disk...");
	for(int block=0;block<FILE_MAX;block++) {
		inode = inode_table.at(block);
		read(fd, buffer, BLOCK_SIZE);
		if(inode->isValid == 1) print("Writing inode for file "+inode->name+" to disk");
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->id).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->isFile).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->isValid).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->blocks_allocated).c_str());
		sprintf(buffer,"%s|%s",buffer,inode->name.c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->indirect_pointer_block).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->double_indirect_pointer_block).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->triple_indirect_pointer_block).c_str());
		for(int i=0;i<DIRECT_BLOCKS;i++) {
			sprintf(buffer,"%s|%s",buffer,std::to_string(inode->direct_blocks[i]).c_str());	
		}
		block_write(block+inode_table_offset,buffer);
	}
	print("done writing inode data to disk");
	close(fd);
	return 0;
}

int read_inode_table_from_vdisk() {
	char buffer[BLOCK_SIZE];
	Inode* inode;
	print("reading inode data from disk...");
	for(int block=0;block<FILE_MAX;block++) {
		inode = new Inode();
		block_read(block+inode_table_offset,buffer);
		std::string s(buffer);
		std::vector<std::string> inode_data = split('|',s);
		inode->id = std::stoi(inode_data.at(1));
		inode->isFile = std::stoi(inode_data.at(2));
		inode->isValid = std::stoi(inode_data.at(3));
		inode->blocks_allocated = std::stoi(inode_data.at(4));
		inode->name = inode_data.at(5);
		inode->indirect_pointer_block = std::stoi(inode_data.at(6));
		inode->double_indirect_pointer_block = std::stoi(inode_data.at(7));
		inode->triple_indirect_pointer_block = std::stoi(inode_data.at(8));
		for(int i=0;i<DIRECT_BLOCKS;i++) {
			inode->direct_blocks[i] = std::stoi(inode_data.at(9+i));
		}
		if(inode->isValid == 1) print("Finished reading inode data for file "+inode->name);
		inode_table.push_back(inode);
	}
	print("done reading inode data from disk");
	return 0;
}

int get_first_free_inode() {
	for(int i=0;i<FILE_MAX;i++) {
		if(inode_table.at(i)->isValid == -1) return i;
	}
}

int create_file(const std::string &name) {
	Inode* inode = inode_table.at(get_first_free_inode());
	inode->blocks_allocated = 1;
	inode->isFile = 1;
	inode->isValid = 1;
	inode->name = name;	
}

int mount_fs(const std::string &diskname) {
	open_disk("vsda");
	read_inode_table_from_vdisk();
	print("Successfully mounted "+diskname);
	return 0;
}

int umount_fs(const std::string &diskname) {
	if(active == 0) {
		print("umount_fs: No disk mounted");
	}
	write_inode_table_to_vdisk();
	close_disk();
	print("Successfully unmounted "+diskname);
	return 0;
}

int find_file(const std::string &filename) { 
	for(int i=0;i<FILE_MAX;i++) {
		if(inode_table.at(i)->name == filename) {
			return inode_table.at(i)->id;
		}
	}
	return -1;
}

int fs_open(const std::string &filename) {
	return find_file(filename);
}

int make_fs(const std::string &s) {

	//don't recreate existing disks
	if(file_exists(s)) {
		print("virtual disk "+s+" already exists, skipping creation");
		return 1;
	} else {
		make_disk(s.c_str());
		print("created virtual disk "+s);
	}

	init_inode_table();

	//open disk to write initial filesystem info
	open_disk("vsda");
	write_inode_table_to_vdisk();
	close_disk();

	return 0;
}