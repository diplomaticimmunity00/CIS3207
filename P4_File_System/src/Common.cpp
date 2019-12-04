#include "Common.h"

int active = 0;
int handle;

int inode_table_offset = 1;
std::vector<Inode*> inode_table;