#pragma once

#define DEF_CORES 1
#define DEF_DISKS 2

#include <iostream>
#include <queue>

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

	int numCores = 3;
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

	template <class T>
	void debug(T info) {
    	std::cout << "Time " << this->clock->get_ticks() << ": " << info << std::endl;
	}

	Simulation(int,int);
	Simulation();
};
