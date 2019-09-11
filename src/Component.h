#pragma once

#include <iostream>

#include "Event.h"

struct Disk {

	int id;	
	bool free;

	ComponentQueue queue;

	bool inline isFree() {return this->free;}
	
	bool operator < (const Disk& disk) const {
		return this->queue.size() > disk.queue.size();
	}

	Disk(int);

};

typedef std::priority_queue<Disk> DiskPriority;

struct CPU {

	int id;
	bool free;
	
	//Job that is currently running, superfluous
	Event job;

	bool inline is_free() {return this->free;}
	bool inline lock() {this->free = false;}

	bool receive_job(Event);
	bool finish_job();

	CPU(int);
};
