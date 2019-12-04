#pragma once
#include <vector>
#include <string>

#include "Common.h"

struct Inode {

	int id;
	short int isValid;
	short int isFile;
	int blocks_allocated;
	std::string name;

	int direct_blocks[DIRECT_BLOCKS];
	
	int indirect_pointer_block = -1;
	int double_indirect_pointer_block = -1;
	int triple_indirect_pointer_block = -1;

	Inode() {
		for(int i=0;i<DIRECT_BLOCKS;i++) direct_blocks[i] = -1;
	}
};