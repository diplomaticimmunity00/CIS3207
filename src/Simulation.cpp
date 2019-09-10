#include "Simulation.h"
#include "Clock.h"

Simulation::Simulation(int cores=DEF_CORES,int disks=DEF_DISKS): numCores(cores), numDisks(disks) {
	this->generate_components();
	this->running = true;
}

Simulation::Simulation(): numCores(DEF_CORES), numDisks(DEF_DISKS) {
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
		if(this->cores.at(i)->is_free()) {
			return i;
		}
	}
	//debug("ERROR: All cores are occupied");
	return -1;
}

void Simulation::start() {
	debug("Simulation starting");
}

void Simulation::dispatch_job(Event e,int coreID) {
	//Creates a new event for process arrival at CPU

	//Prevent core from scheduling subsequent jobs	
	this->get_core(coreID)->lock();

	//Create event for process arriving to cpu
	Event _e = Event(PROCESS_ARRIVE_CPU);
	_e.process = e.process;
	_e.process->targetCore = coreID;
	_e.time = e.time;
	this->enqueue(_e);

	debug("PID "+std::to_string(_e.process->id)+" queued for core "+std::to_string(coreID));
}

void Simulation::handle_system_arrival(Event e) {
	//e.time = this->clock->get_ticks();

	debug("PID "+std::to_string(e.process->id)+" enters system");

	//Check queues and cores
	int freecore = this->get_first_free_core();
	if(freecore >= 0 and this->cpuQueue.empty()) {
		
		this->dispatch_job(e,freecore);

	} else {
		debug("Pushed PID "+std::to_string(e.process->id)+" to CPU queue");
		this->cpuQueue.push(e);
	}

	//Create new process
	this->enqueue(PROCESS_ARRIVAL);
}

void Simulation::handle_cpu_arrival(Event e) {

	//Send job to target core 
	//Target core ensured free as it was locked when this process was queued for arrival
	this->cores.at(e.process->targetCore)->receive_job(e);
	
	//Create event for process ending on cpu
	Event _e = Event(PROCESS_FINISH_CPU);
	_e.process = e.process;

	//Advancing time here presumes threading
	//Advance time here for other components
	//_e.time = this->clock->get_ticks() + rand()%30;
	
	//Set to highest possible priority
	_e.time = this->clock->get_ticks();
	//_e.time = e.time;
	this->enqueue(_e);
}

void Simulation::handle_cpu_exit(Event e) {

	//Simulate CPU running some program (time placeholder)
	this->clock->step(rand()%34);

	this->cores.at(e.process->targetCore)->finish_job();
	debug("Killing process "+std::to_string(e.process->id)+" for now...\n");

	if(!this->cpuQueue.empty()) {

		//When cpu finishes, pull next process off queue
		Event nextProcessInQueue = cpuQueue.front();
		cpuQueue.pop();

		debug("Pulling PID "+std::to_string(nextProcessInQueue.process->id)+" off of CPU queue...");

		int freecore = this->get_first_free_core();//nextProcessInQueue.process->targetCore;

		this->dispatch_job(nextProcessInQueue,freecore);
	}

}

void Simulation::process_from_queue() {
	if(this->eventQueue.empty()) {
		return;
	}
	Event e = this->eventQueue.top();
	this->eventQueue.pop();
	switch(e.type) {
		case PROCESS_ARRIVAL:
			//Process enters system
			this->handle_system_arrival(e);
			break;
		case PROCESS_ARRIVE_CPU:
			//Process arrives at a core
			this->handle_cpu_arrival(e);
			break;
		case PROCESS_FINISH_CPU:
			//Process finishes running on a core
			this->handle_cpu_exit(e);
			break;
		default:
			debug("Unhandled case for event type "+std::to_string(e.type));
	}
	this->clock->step();
}
