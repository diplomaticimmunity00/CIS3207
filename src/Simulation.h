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

	unsigned short pids = 0;

	Clock* clock = new Clock(0);

	std::vector<CPU*> cores;
	std::vector<Disk*> disks;

	ComponentQueue cpuQueue;
	EventQueue eventQueue;

	CPU* controlCore;

	int numCores;
	int numDisks;

	void start();

	//Enqueues existing job
	void enqueue(Event);
	//Creates new job with specified type
	void enqueue(EventType);

	//COMPONENTS
	void generate_components();
	void assign_control_core(int);
	int get_control_core_id();
	inline CPU* get_core(int i) {return this->cores.at(i); }
	int get_first_free_core();

	void handle_disk_arrival(Event);
	void handle_disk_exit(Event);
	int get_first_free_disk();
	int get_best_disk_queue();
	inline Disk* get_disk(int i) {return this->disks.at(i);}

	//SYSTEM
	void handle_system_arrival(Event);
	void handle_system_exit(Event);

	//CPU
	void handle_cpu_arrival(Event);
	void dispatch_job(Event,ComponentType,int);
	void handle_cpu_exit(Event);
	void process_from_queue();

	template <class T>
	void debug(T info,bool timestamp = true) {
		if(timestamp) {
   			std::cout << "Time " << this->clock->get_ticks() << ": " << info << std::endl;
		} else {
			std::cout << "Time " << this->clock->get_ticks() << ": " << info << std::endl;
		}
	}

	Simulation(int,int);
	Simulation();
};
