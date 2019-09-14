#pragma once

#include <iostream>
#include <queue>

#include "Component.h"
#include "Event.h"
#include "Clock.h"
#include "Config.h"

struct Simulation {

	bool running;

	unsigned short pids = 0;

	Clock* clock = new Clock();
	Config* config = new Config();

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

	void dequeue();

	void CPUenqueue(Event);
	void CPUdequeue();

	unsigned int queueAccesses = 0;
    unsigned int maxQueueSize = 0;
    unsigned int totalEventsSeen = 0;

	unsigned int CPUqueueAccesses = 0;
    unsigned int CPUmaxQueueSize = 0;
    unsigned int CPUtotalEventsSeen = 0;

    float get_average_queue_size() {
        if(this->totalEventsSeen == 0) return -1;
        return this->queueAccesses*1.0f/this->totalEventsSeen;
 	}
	float get_average_cpu_queue_size() {
        if(this->CPUtotalEventsSeen == 0) return -1;
        return this->CPUqueueAccesses*1.0f/this->CPUtotalEventsSeen;
    }

	//COMPONENTS
	//* CPU
	void generate_components();
	void assign_control_core(int);
	int get_control_core_id();
	inline CPU* get_core(int i) {return this->cores.at(i); }
	int get_first_free_core();

	//* DISK
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
			std::cout << info << std::endl;
		}
	}

	void get_stats() {
		CPU* testCore;
		Disk* testDisk;
		
		debug("\n=========",false);
		debug("==STATS==",false);
		debug("=========",false);

		debug("\n==Simulation==",false);
        debug("Exit time: "+std::to_string(this->clock->get_ticks()),false);
        debug("Processes introduced: "+std::to_string(this->pids),false);

		debug("\n==Utilization==",false);

		uint32_t totalTime = this->clock->get_ticks()-this->config->get_config_value(INIT_TIME);

		for(int i=0;i<this->numCores;i++) {
			testCore = this->get_core(i);
			debug("CORE "+std::to_string(testCore->id) + ": "+std::to_string(1.0f*testCore->totalUsageTime/totalTime),false);
		}
		debug("",false);
		for(int i=0;i<this->numDisks;i++) {
			testDisk = this->get_disk(i);
			debug("DISK "+std::to_string(testDisk->id) + ": "+std::to_string(1.0f*testDisk->totalUsageTime/totalTime),false);
		}

		debug("\n==Response Time==",false);

		for(int i=0;i<this->numCores;i++) {
			testCore = this->get_core(i);
			debug("CORE "+std::to_string(testCore->id) + ": Max "+std::to_string(testCore->maxResponseTime)+", Avg "+std::to_string(testCore->get_avg_response_time()),false);
		}
		debug("",false);
		for(int i=0;i<this->numDisks;i++) {
			testDisk = this->get_disk(i);
			debug("DISK "+std::to_string(testDisk->id) + ": Max "+std::to_string(testDisk->maxResponseTime)+", Avg "+std::to_string(testDisk->get_avg_response_time()),false);
		}

		debug("\n==Throughput== (jobs per tick)",false);

		for(int i=0;i<this->numCores;i++) {
			testCore = this->get_core(i);
			debug("CORE "+std::to_string(testCore->id) + ": "+std::to_string(1.0f*testCore->totalJobs/totalTime),false);
		}
		debug("",false);
		for(int i=0;i<this->numDisks;i++) {
			testDisk = this->get_disk(i);
			debug("DISK "+std::to_string(testDisk->id) + ": "+std::to_string(1.0f*testDisk->totalJobs/totalTime),false);
		}

		debug("\n==Queue Sizes==",false);
		debug("Event: Max "+std::to_string(this->maxQueueSize) + ", Avg: "+std::to_string(this->get_average_queue_size()),false);
		debug("CPU: Max "+std::to_string(this->CPUmaxQueueSize) + ", Avg: "+std::to_string(this->get_average_cpu_queue_size()),false);
		
		for(int i=0;i<this->numDisks;i++) {
			testDisk = this->get_disk(i);
			debug("DISK "+std::to_string(testDisk->id) + ": "+std::to_string(testDisk->get_average_queue_size()),false);
		}
	}

	Simulation(int,int);
	Simulation();

	~Simulation() {
		this->debug("Exiting simulation");
		this->get_stats();
		delete this->config;
		delete this->clock;
	}
};
