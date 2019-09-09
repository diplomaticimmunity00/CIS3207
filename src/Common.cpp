#include "Common.h"
#include <ctime>

Event::Event(EventType _type): type(_type)  {}

CPU::CPU(int _id): id(_id) {
}

Disk::Disk(int _id): id(_id) {
}

Simulation::Simulation() {
	this->generate_components();
	this->running = true;
}

void Simulation::generate_components() {

	//Generate cores
	for(int i=0;i<this->numCores;i++) {
		this->cores.push_back(new CPU(i));
	}
	//Generate disks
	for(int i=0;i<this->numDisks;i++) {
        this->disks.push_back(new Disk(i));
    }
}

void Simulation::start() {
	std::cout << "Simulation starting" << std::endl;
	std::cout << clock() << std::endl;
}

void Simulation::add_event(Event e) {
	
}

void Simulation::process_event() {
	std::cout << "Processing event" << std::endl;
}
