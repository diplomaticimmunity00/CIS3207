#include <iostream>
#include "Common.h"

int main() {

	Simulation sim = Simulation();
	sim.start();

	int i = 0;

	while(sim.running) {
		sim.process_event();
		i++;
		if(i >= 100) {
			sim.running = false;
			break;
		}
	}

	return 0;
}
