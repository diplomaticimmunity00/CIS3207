#include "Simulation.h"
#include "Common.h"
#include <ctime>

CPU::CPU(int _id): id(_id) {
	this->free = true;
}

bool CPU::receive_job(Event e) {
	this->job = e;
	sim.debug("Received PID "+std::to_string(e.process->id)+" at core "+std::to_string(this->id));
	this->free = false;
	return true;
}

bool CPU::finish_job() {
	sim.debug("Finished PID "+std::to_string(this->job.process->id)+" on core "+std::to_string(this->id));
	this->free = true;
	return true;
}

Disk::Disk(int _id): id(_id) {
}
