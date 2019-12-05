#include "Filesys.h"
#include "Utility.h"
#include "Common.h"
#include "Inode.h"

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "Common.h"
#include "disk.h"

void init_disk() {
	//zero disk
	char buffer[BLOCK_SIZE];
	clear_buffer(buffer,BLOCK_SIZE);
	for(int i=0;i<DISK_BLOCKS;i++) {
		block_write(i,buffer);
	}
}

int init_inode_table() {
	//initializes first FILE_MAX blocks on the virtual disk with space for inodes
	for(int block=0;block<FILE_MAX;block++) {
		Inode* inode = new Inode();
		inode->id = block+1;
		inode->size = 0;
		inode->blocks_allocated = 0;
		inode->isFile = 1;
		inode->isValid = 0;
		inode->name = "FNAME_UNDEF";
		inode_table.push_back(inode);
	}
	return 0;
}

int write_inode_table_to_vdisk() {
	char buffer[BLOCK_SIZE];
	Inode* inode;
	print("writing inode data to disk...");
	int valid_files = 0;
	for(int block=0;block<FILE_MAX;block++) {
		inode = inode_table.at(block);
		clear_buffer(buffer,BLOCK_SIZE);
		if(inode->isValid == 1) {
			valid_files++;
			if(inode->isFile) print("Writing inode for file "+inode->name+" to disk at inode "+std::to_string(inode->id));
			if(!inode->isFile) print("Writing inode for directory "+inode->name+" to disk at inode "+std::to_string(inode->id));
		}
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->id).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->size).c_str());
		sprintf(buffer,"%s|%s",buffer,std::to_string(inode->pointer).c_str());
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
	print("done writing inode data to disk. wrote "+std::to_string(valid_files)+" files.");
	return 0;
}

int read_inode_table_from_vdisk() {
	char buffer[BLOCK_SIZE];
	Inode* inode;
	int valid_files = 0;
	print("reading inode data from disk...");
	for(int block=0;block<FILE_MAX;block++) {
		inode = new Inode();
		block_read(block+inode_table_offset,buffer);
		std::string s(buffer);
		std::vector<std::string> inode_data = split('|',s);
		inode->id = std::stoi(inode_data.at(1));
		inode->size = std::stoi(inode_data.at(2));
		inode->pointer = std::stoi(inode_data.at(3));
		inode->isFile = std::stoi(inode_data.at(4));
		inode->isValid = std::stoi(inode_data.at(5));
		inode->blocks_allocated = std::stoi(inode_data.at(6));
		inode->name = inode_data.at(7);
		inode->indirect_pointer_block = std::stoi(inode_data.at(8));
		inode->double_indirect_pointer_block = std::stoi(inode_data.at(9));
		inode->triple_indirect_pointer_block = std::stoi(inode_data.at(10));
		for(int i=0;i<DIRECT_BLOCKS;i++) {
			inode->direct_blocks[i] = std::stoi(inode_data.at(11+i));
		}
		if(inode->isValid == 1) {
			valid_files++;
			if(inode->isFile) print("Finished reading inode data for file "+inode->name+" at inode "+std::to_string(inode->id));	
			if(!inode->isFile) print("Finished reading inode data for directory "+inode->name+" at inode "+std::to_string(inode->id));
		}
		inode_table.push_back(inode);
	}
	print("done reading inode data from disk. read "+std::to_string(valid_files)+" files.");
	return 0;
}

int get_first_free_inode() {
	for(int i=0;i<FILE_MAX;i++) {
		if(inode_table.at(i)->isValid == 0) return i;
	}
}

int fs_create(const std::string &name) {
	if(name.size() > 15) {
		print("fs_create: File name '"+name+"'too long, aborting");
		return -1;
	}
	Inode* inode = inode_table.at(get_first_free_inode());
	inode->isFile = 1;
	inode->isValid = 1;
	inode->name = name;
	return 0;
}

int free_blocks(int file_inode_index) {
	char buffer[BLOCK_SIZE];
	clear_buffer(buffer,BLOCK_SIZE);
	Inode *inode = inode_table.at(file_inode_index);
	int i = 0;
	for(i;i<inode->blocks_allocated;i++) {
		int block =inode->direct_blocks[i];
		if(block >= 0) block_write(block,buffer);
	}
	return i;
}

int fs_delete(const std::string &path) {
	int file_inode_index = get_file_index(path);
	if(file_inode_index < 0) {
		print("fs_delete: File '"+path+"' not found");
		return -1;
	}
	Inode* inode = inode_table.at(file_inode_index);
	int freed_blocks = free_blocks(file_inode_index);
	inode->blocks_allocated = 0;
	inode->isValid = 0;
	print("Deleted file "+path+" at inode "+std::to_string(inode->id)+". Freed "+std::to_string(freed_blocks)+" blocks.");
	return 0;
}

int fs_mkdir(const std::string &name) {
	if(name.size() > 15) {
		print("fs_mkdir: Directory name '"+name+"'too long, aborting");
		return -1;
	}
	Inode* inode = inode_table.at(get_first_free_inode());
	inode->isFile = 0;
	inode->isValid = 1;
	inode->name = name;
	return 0;
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

int get_file_index(const std::string &filename) { 
	//returns inode index of file with name filename
	for(int i=0;i<FILE_MAX;i++) {
		if(inode_table.at(i)->isValid == 0) continue;
		if(inode_table.at(i)->name == filename) {
			return i;
		}
	}
	return -1;
}

int get_file_index_by_fd(int fd) {
	if(file_table.find(fd) != file_table.end()) {
		return file_table.at(fd);
	}
	return -1;
}

std::string get_file_name_by_fd(int fd) {
	if(file_table.find(fd) != file_table.end()) {
		return inode_table.at(file_table.at(fd))->name;
	}
	return "ERR_FD_NOT_FOUND";
}

int get_first_free_block() {
	char buffer[BLOCK_SIZE];
	char tbuffer[BLOCK_SIZE];
	clear_buffer(tbuffer,BLOCK_SIZE);
	for(int i=inode_table_offset;i<DISK_BLOCKS;i++) {
		block_read(i,buffer);
		//if block is empty/unmodified
		if(strncmp(buffer,tbuffer,BLOCK_SIZE) == 0) {
			print("FOUND FREE BLOCK "+std::to_string(i));
			return i;
		}
	}
	print("NO FREE BLOCKS?!?!?");
	return -1;
}

int allocate_new_block(Inode *inode) {
	int block = get_first_free_block();

	//allocate indirect blocks here
	inode->direct_blocks[inode->blocks_allocated] = block;
	inode->blocks_allocated++;
	print("allocated new block "+std::to_string(block)+" to inode "+std::to_string(inode->id));
	return block;
}

int get_active_block(int fd) {
	int file_inode_index = get_file_index_by_fd(fd);
	if(file_inode_index < 0) return -1;
	Inode *inode = inode_table.at(file_inode_index);
	if(inode->blocks_allocated == 0) {
		//allocate a new block to inode fd
		int new_block = allocate_new_block(inode);
	}
	//identify last block in inode block hierarchy
	return inode->direct_blocks[inode->blocks_allocated-1];
}

int fs_write(int fd, char *buf, size_t nbytes) {

	char write_buffer[nbytes];
	clear_buffer(write_buffer,nbytes);
	for(int i=0;i<nbytes;i++) write_buffer[i] = buf[i];

	int active_block = get_active_block(fd);
	if(active_block < 0) return -1;
	Inode *inode = inode_table.at(get_file_index_by_fd(fd));

	char blockContent[BLOCK_SIZE];
	clear_buffer(blockContent,BLOCK_SIZE);

	block_read(active_block,blockContent);

	//write what we can to the current block
	// remove what was written from the input buffer
	// allocate a new block
	// etc.

	int required_bytes = std::min(truesize(write_buffer,nbytes),(int)nbytes);
	int written;

	while(truesize(blockContent,BLOCK_SIZE)+required_bytes > BLOCK_SIZE) {
		print("OVERFLOW from block "+std::to_string(active_block)+". allocating new block...");
		int space_avail = BLOCK_SIZE - truesize(blockContent,BLOCK_SIZE);
		if(space_avail > 0) {
			written = fs_write(fd,write_buffer,space_avail);
			required_bytes -= written;
		}

		allocate_new_block(inode);
		active_block = get_active_block(fd);
		clear_buffer(blockContent,BLOCK_SIZE);
		block_read(active_block,blockContent);

		buffer_erase(write_buffer,nbytes,written);

	}

	print("ATTEMPTING TO WRITE "+std::to_string(truesize(write_buffer,nbytes)) + " BYTES TO BLOCK WITH SIZE "+std::to_string(truesize(blockContent,BLOCK_SIZE)));

	print("NEW BLOCK SIZE "+std::to_string(truesize(blockContent,BLOCK_SIZE)));

	block_write(active_block,write_buffer,inode->pointer);

	print("wrote "+std::to_string(required_bytes)+" bytes to block "+std::to_string(active_block)+" for file "+get_file_name_by_fd(fd));

	inode->size += required_bytes;
	inode->pointer = inode->size;
	print("NEW FILESIZE: "+std::to_string(inode->size));
	print("NEW BLOCK COUNT: "+std::to_string(inode->blocks_allocated));
	return required_bytes;
}

int fs_read(int fd, char *buf, size_t nbyte) {
	char file_contents[nbyte]; 		//target buffer to read file contents into
	char tbuffer[BLOCK_SIZE];		//temp buffer for reading from active blocks
	clear_buffer(file_contents,nbyte);

	//get inode from fd
	int file_index = get_file_index_by_fd(fd);
	if(file_index < 0) return -1;
	Inode *inode = inode_table.at(file_index);
	
	//traverse through inode to get file contents
	for(int i=0;i<inode->blocks_allocated;i++) {
		int block = inode->direct_blocks[i];
		if(block >= 0 and block <= DISK_BLOCKS) {
			clear_buffer(tbuffer,BLOCK_SIZE);
			block_read(block,tbuffer);
			sprintf(file_contents,"%s%s",file_contents,tbuffer);
		}
	}
	sprintf(buf,"%s",file_contents);
	return 0;
}

int fs_get_filesize(int fd) {
	int index = get_file_index_by_fd(fd);
	return inode_table.at(index)->size;
}

int fs_lseek(int fd, size_t offset) {
	Inode *inode = inode_table.at(get_file_index_by_fd(fd));
	if(offset > fs_get_filesize(fd) or offset < 0) return -1;
	inode->pointer = offset;
	return 0;
}

int fs_truncate(int fd, size_t len) {
	//writes null characters over last len bytes of file
	Inode *inode = inode_table.at(get_file_index_by_fd(fd));
	if(len > fs_get_filesize(fd) or len < 0) return -1;
	char buffer[inode->size-len];
	clear_buffer(buffer,inode->size-len);
	inode->pointer = len;
	fs_write(fd,buffer,inode->size-len);	
	inode->size = len;
	return 0;
}

int fs_open(const std::string &filename) {
	int file_inode_index = get_file_index(filename);
	if(file_inode_index < 0) {
		print("fs_open: File '"+filename+"' could not be opened");
		return -1;
	}
	int fd = file_table.size();
	file_table.insert({fd,file_inode_index});
	return fd;
}

int fs_close(int fd) {
	if(file_table.find(fd) == file_table.end()) {
		print("fs_close: File descriptor "+std::to_string(fd)+" not found or not open");
		return -1;
	}
	file_table.erase(fd);
	return 0;
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
	init_disk();
	write_inode_table_to_vdisk();
	close_disk();

	return 0;
}