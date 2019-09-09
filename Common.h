#include <iostream>
#include <queue>

enum PID {

		PROCESS_ARRIVAL,
		PROCESS_ARRIVE_CPU,
		PROCESS_FINISH_CPU,
		PROCESS_ARRIVE_DISK1,
		PROCESS_ARRIVE_DISK2,
		PROCESS_FINISH_DISK1,
		PROCESS_FINISH_DISK2,
		SIMULATION_FINISH,
};

struct Event {

    int id;
    uint32_t arrived;

};

struct Process {

    int id;
    uint32_t start_time;
    uint32_t fin_time;

};

struct Disk {

	int id;	
	std::priority_queue<Event> queue;
};

struct CPU {

	int id;
	std::priority_queue<Event> queue;

};

struct Simulation {

	bool running;

	void start();
	void process_event();

	Simulation();
};
