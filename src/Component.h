#pragma once

#include <iostream>
#include <map>

#include "Event.h"

enum ComponentType {
		COMPONENT_CPU,
		COMPONENT_DISK
};

extern std::map<ComponentType,std::string> componentMap;

struct Component {

	ComponentType type;
	
	int id;
	bool free;

	Event job;

	bool inline is_free() {return this->free;}
	void inline lock() {this->free = false;}

	virtual void receive_job(Event);
	virtual void complete_job();

	Component(ComponentType,int);
};

struct Disk : Component {

	ComponentQueue queue;

	bool operator < (const Disk& disk) const {
		return this->queue.size() > disk.queue.size();
	}

	Disk(int);

};

typedef std::priority_queue<Disk> DiskPriority;

struct CPU : Component {

	CPU(int);

};
