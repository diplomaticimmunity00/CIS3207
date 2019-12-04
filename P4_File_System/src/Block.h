#pragma once
#include "disk.h"

struct PointerBlock {
	int blocks[BLOCK_SIZE/sizeof(unsigned short)];
};