#pragma once

#include <iostream>
#include <queue>

#include "Simulation.h"

enum EventType {

		PROCESS_ARRIVAL,
		PROCESS_EXIT,
		PROCESS_ARRIVE_CPU,
		PROCESS_FINISH_CPU,
		PROCESS_ARRIVE_DISK,
		PROCESS_FINISH_DISK,
		SIMULATION_FINISH,
};

/*	defunct
enum ProcessStatus {
	PROCESS_EXECUTING,
	PROCESS_IO,
	PROCESS_FINISHED
};
*/

struct Process {

    int id;
	int targetComponentID;
	int cpuTime;
	int ioTime;
	//ProcessStatus status;

	Process();

};

struct Event {

    int id;
	Process* process;
	EventType type;
    uint32_t time;

	bool operator < (const Event& e) const {
		//Processes with a smaller time value are higher up in the queue
		return this->time > e.time;
	}

	Event(EventType);
	Event() {}
};

typedef std::priority_queue<Event> EventQueue;
typedef std::queue<Event> ComponentQueue;
