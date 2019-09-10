#pragma once

#include <iostream>
#include <queue>

template <class T>
void debug(T info) {
    std::cout << info << std::endl;
}

#include "Component.h"
#include "Event.h"
#include "Clock.h"

struct Simulation {

	bool running;

	int pids = 0;

	Clock* clock = new Clock(0);

	std::vector<CPU*> cores;
	std::vector<Disk*> disks;

	EventQueue cpuQueue;
	EventQueue eventQueue;

	int numCores = 1;
	int numDisks = 2;

	void start();

	//Enqueues existing job
	void enqueue(Event);
	//Creates new job with specified type
	void enqueue(EventType);

	//Components
	void generate_components();
	int get_first_free_core();

	void handle_event_arrival(Event);
	void handle_cpu_arrival(Event);
	void handle_cpu_exit(Event);
	void process_from_queue();

	Simulation();
};
