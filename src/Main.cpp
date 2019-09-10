#include <iostream>
#include <ctime>

#include "Common.h"


/*
 *	enum EventType 
 *      PROCESS_ARRIVAL,
 *      PROCESS_ARRIVE_CPU,
 *      PROCESS_FINISH_CPU,
 *      PROCESS_ARRIVE_DISK1,
 *      PROCESS_ARRIVE_DISK2,
 *      PROCESS_FINISH_DISK1,
 *      PROCESS_FINISH_DISK2,
 *      SIMULATION_FINISH,
 *	*/


int main() {

	srand(time(NULL));

	sim.enqueue(PROCESS_ARRIVAL);
	sim.enqueue(PROCESS_ARRIVAL);
	sim.enqueue(PROCESS_ARRIVAL);
	sim.enqueue(PROCESS_ARRIVAL);
    sim.enqueue(PROCESS_ARRIVAL);
    sim.enqueue(PROCESS_ARRIVAL);
	sim.start();
	
	debug(sim.eventQueue.top().time);
    sim.eventQueue.pop();
    debug(sim.eventQueue.top().time);
    sim.eventQueue.pop();
	debug(sim.eventQueue.top().time);
	sim.eventQueue.pop();
	debug(sim.eventQueue.top().time);
	sim.eventQueue.pop();
    debug(sim.eventQueue.top().time);
	
	exit(0);

	int i = 0;
	while(sim.running) {
		sim.process_from_queue();
		i++;
		if(i > 100) sim.running = false;
	}

	return 0;
}
