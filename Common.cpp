#include "Common.h"
#include <ctime>

Simulation::Simulation() {
	running = true;
}

void Simulation::start() {
	std::cout << "Simulation starting" << std::endl;
	std::cout << clock() << std::endl;
}

void Simulation::process_event() {
	std::cout << "Processing event" << std::endl;
}
