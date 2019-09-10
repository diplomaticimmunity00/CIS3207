#pragma once

#include <iostream>

#include "Event.h"

struct Disk {

	int id;	
	bool free;

	EventQueue queue;

	bool inline isFree() {return this->free;}

	Disk(int);

};

struct CPU {

	int id;
	bool free;
	
	Event job;

	bool inline isFree() {return this->free;}

	bool receive_job(Event);
	bool finish_job();

	CPU(int);
};
