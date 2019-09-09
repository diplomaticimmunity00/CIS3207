#include <iostream>
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

	Simulation sim = Simulation();

	sim.add_event(PROCESS_ARRIVAL);

	sim.start();

	int i = 0;

	while(sim.running) {
		sim.process_event();
		i++;
		if(i > 100) sim.running = false;
	}

	return 0;
}
