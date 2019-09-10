#pragma once

#include <iostream>
#include <queue>

#include "Simulation.h"

enum EventType {

		PROCESS_ARRIVAL,
		PROCESS_ARRIVE_CPU,
		PROCESS_FINISH_CPU,
		PROCESS_ARRIVE_DISK1,
		PROCESS_ARRIVE_DISK2,
		PROCESS_FINISH_DISK1,
		PROCESS_FINISH_DISK2,
		SIMULATION_FINISH,
};

enum ProcessStatus {
	PROCESS_EXECUTING,
	PROCESS_IO,
	PROCESS_FINISHED
};

struct Process {

    int id;
	int targetCore;
	ProcessStatus status;

	Process();

};

struct Event {

    int id;
	Process* process;
	EventType type;
    uint32_t time;

	bool operator < (const Event& e) const {
		return this->time < e.time;
	}

	Event(EventType);
	Event() {}
};

typedef std::priority_queue<Event> EventQueue;
