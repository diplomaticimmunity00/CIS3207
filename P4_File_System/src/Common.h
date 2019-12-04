#pragma once
#define FILE_MAX 256
#define DIRECT_BLOCKS 12
#include <vector>

#include "Inode.h"

extern int active;
extern int handle;

struct Inode;

extern int inode_table_offset;
extern std::vector<Inode*> inode_table;//[FILE_MAX*BLOCK_SIZE];