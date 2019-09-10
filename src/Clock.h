#pragma once

#include <iostream>

struct Clock {

	uint32_t ticks;

	inline uint32_t get_ticks() { return ticks; }
	inline void step() { this->ticks++; }

	Clock(uint32_t start): ticks(start) {}
};
