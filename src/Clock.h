#pragma once

#include <iostream>

struct Clock {

	uint32_t ticks;

	inline uint32_t get_ticks() { return ticks; }
	inline void step() { this->ticks++; }
	inline void step(int i) {this->ticks = this->ticks + i;}

	Clock(uint32_t start): ticks(start) {}
};
