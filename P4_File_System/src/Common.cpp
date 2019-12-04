#include "Common.h"

int active = 0;
int handle;

int inode_table_offset = 1;

std::map<int,int> file_table;
std::vector<Inode*> inode_table;