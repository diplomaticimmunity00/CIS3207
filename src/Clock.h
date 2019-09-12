#pragma once

#include <iostream>

struct Clock {

	uint32_t ticks;

	inline uint32_t get_ticks() { return ticks; }
	inline void step() { this->ticks++; }
	inline void step(int i) {this->ticks = this->ticks + i;}
	inline void set_time(int i) {this->ticks = i;}

	Clock() {}
};
