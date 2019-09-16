#include <iostream>
#include <ctime>

#include "Common.h"

int main() {

	sim.enqueue(PROCESS_ARRIVAL);
	sim.enqueue(PROCESS_ARRIVAL);
	sim.enqueue(PROCESS_ARRIVAL);

	sim.start();

	while(sim.running) {
		sim.process_from_queue();
		if(sim.clock->get_ticks() > sim.config->get_config_value(FIN_TIME)) sim.running = false;
	}

	return 0;
}
