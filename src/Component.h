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

	uint32_t totalUsageTime = 0;
	int totalJobs = 0;
	int maxResponseTime = 0;

	unsigned int queueAccesses = 0;
	unsigned int maxQueueSize = 0;
	unsigned int totalEventsSeen = 0;

	float get_average_queue_size() {
		if(this->totalEventsSeen == 0) return -1;
		return this->queueAccesses*1.0f/this->totalEventsSeen;
	}

	Event job;

	int inline get_avg_response_time() {
		if(this->totalJobs == 0) {
			return -1;
		}
		return this->totalUsageTime*1.0f/this->totalJobs;
	}

	int inline get_max_response_time() {return this->maxResponseTime;}

	bool inline is_free() {return this->free;}
	void inline lock() {this->free = false;}

	virtual void receive_job(Event);
	virtual void complete_job();

	Component(ComponentType,int);
};

struct Disk : Component {

	ComponentQueue queue;

	void enqueue(Event e) {
		this->queueAccesses++;
		this->queue.push(e);
		this->totalEventsSeen++;
		if(this->queue.size() > this->maxQueueSize) {
			this->maxQueueSize = this->queue.size();
		}
	}

	void dequeue() {
		this->queueAccesses++;
		this->queue.pop();
	}

	bool operator < (const Disk& disk) const {
		return this->queue.size() > disk.queue.size();
	}

	virtual void complete_job();


	Disk(int);

};

typedef std::priority_queue<Disk> DiskPriority;

struct CPU : Component {
	
	virtual void complete_job();


	CPU(int);

};
