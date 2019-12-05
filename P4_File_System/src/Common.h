#pragma once
#define FILE_MAX 256
#define OPEN_FILE_MAX 64
#define DIRECT_BLOCKS 24
#include <vector>
#include <map>

#include "Inode.h"

extern std::string cwd;

extern int active;
extern int handle;

struct Inode;

extern int inode_table_offset;

//File table is a map of unique file descriptors to associated file-ids. 
// Multiple unique file descriptors can point to the same file-id
extern std::map<int,int> file_table;
extern std::vector<Inode*> inode_table;//[FILE_MAX*BLOCK_SIZE];