#pragma once
#include <vector>
#include <map>
#include <string>

#include "Common.h"

struct Inode {

	int id; //unique (static) inode ID
			// represents location in inode table +1
	int size = 0; //size on disk;
	short int isValid = 0; //currently represents a real file
	short int isFile = 0; //file or directory

	int pointer = 0; //file pointer (cursor)

	int blocks_allocated = 0; //number of blocks allocated to this file
	std::string name; //filename

	//maps filenames to inode (file) indicies in inode table
	std::map<std::string,int> contents;

	int direct_blocks[DIRECT_BLOCKS];
	
	int indirect_pointer_block = -1;
	int double_indirect_pointer_block = -1;
	int triple_indirect_pointer_block = -1;

	Inode() {
		for(int i=0;i<DIRECT_BLOCKS;i++) direct_blocks[i] = -1;
	}
};