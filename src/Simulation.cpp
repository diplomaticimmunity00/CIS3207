#include "Simulation.h"
#include "Clock.h"

Simulation::Simulation() {
	this->generate_components();
	this->running = true;
}

void Simulation::enqueue(Event e) {
	this->eventQueue.push(e);
}
void Simulation::enqueue(EventType e) {
	Event _e = Event(e);
												//Placeholder
	_e.time = this->clock->get_ticks() + rand()%30;
	this->clock->step();
	this->eventQueue.push(_e);
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

int Simulation::get_first_free_core() {
	//Returns -1 if all cores are occupied
	for(int i=0;i<this->numCores;i++) {
		if(this->cores.at(i)->isFree()) {
			return i;
		}
	}
	return -1;
}

void Simulation::start() {
	debug("Simulation starting");
}

void Simulation::handle_event_arrival(Event e) {
	e.time = this->clock->get_ticks();

	//Check queues and cores
	int freecore = this->get_first_free_core();
	if(freecore >= 0 and this->cpuQueue.empty()) {
		
		//Prevent core from scheduling subsequent jobs	
		this->cores.at(freecore)->free = false;

		//Create event for process arriving to cpu
		Event _e = Event(PROCESS_ARRIVE_CPU);
		_e.process = e.process;
		_e.process->targetCore = freecore;
		_e.time = this->clock->get_ticks();
		this->enqueue(_e);
	} else {
		this->cpuQueue.push(e);
	}

	//Create new process
	this->enqueue(PROCESS_ARRIVAL);
}

void Simulation::handle_cpu_arrival(Event e) {


	//Send job to target core 
	this->cores.at(e.process->targetCore)->receive_job(e);
	
	//Create event for process ending on cpu
	Event _e = Event(PROCESS_FINISH_CPU);
	_e.process = e.process;
	_e.time = this->clock->get_ticks() + rand()%30;
	this->enqueue(_e);

}

void Simulation::handle_cpu_exit(Event e) {

	this->cores.at(e.process->targetCore)->finish_job();
	debug("Killing process "+std::to_string(e.process->id)+" for now...");
}

void Simulation::process_from_queue() {
	if(this->eventQueue.empty()) {
		return;
	}
	Event e = this->eventQueue.top();
	this->eventQueue.pop();
	switch(e.type) {
		case PROCESS_ARRIVAL:
			this->handle_event_arrival(e);
			break;
		case PROCESS_ARRIVE_CPU:
			this->handle_cpu_arrival(e);
			break;
		case PROCESS_FINISH_CPU:
			this->handle_cpu_exit(e);
			break;
		default:
			debug("Unhandled case for event type "+std::to_string(e.type));
	}
}
