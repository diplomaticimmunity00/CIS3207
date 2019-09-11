#include <iostream>
#include <ctime>

#include "Common.h"


/*
 *	enum EventType 
 *      PROCESS_ARRIVAL,
 *      PROCESS_EXIT,
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

	sim.start();

/*
 * Queue priority debug
	sim.debug(sim.eventQueue.top().time);
	sim.eventQueue.pop();
	sim.debug(sim.eventQueue.top().time);
    sim.eventQueue.pop();
	sim.debug(sim.eventQueue.top().time);
    sim.eventQueue.pop();
	exit(0);
*/

	while(sim.running) {
		sim.process_from_queue();
		if(sim.clock->get_ticks() > 200) sim.running = false;
	}

	return 0;
}
