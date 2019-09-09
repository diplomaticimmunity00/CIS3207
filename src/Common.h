#include <iostream>
#include <queue>

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
	ProcessStatus status;
    uint32_t start_time;
    uint32_t fin_time;

};

struct Event {

    int id;
	Process proc;
	EventType type;
    uint32_t arrived;

	Event(EventType);
};

struct Disk {

	int id;	
	std::priority_queue<Event> queue;

	Disk(int);

};

struct CPU {

	int id;
	bool free;
	std::priority_queue<Event> queue;

	CPU(int);
};

struct Simulation {

	bool running;

	std::vector<CPU*> cores;
	std::vector<Disk*> disks;

	int numCores = 1;
	int numDisks = 2;

	void start();
	void generate_components();
	void add_event(Event);
	void process_event();

	Simulation();
};
