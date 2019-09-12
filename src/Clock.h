#pragma once

#include <iostream>
#include <chrono>
#include <thread>

struct Clock {

	uint32_t ticks;
	bool realtime;
	int tickrate;

	inline uint32_t get_ticks() { return ticks; }
	inline void step() { this->step(1); }
	inline void step(int i) {
		if(this->realtime) std::this_thread::sleep_for(std::chrono::milliseconds(i*(1000/tickrate)));	
		this->ticks = this->ticks + i;
	}	
	inline void set_time(int i) {this->ticks = i;}

	Clock() {}
};
