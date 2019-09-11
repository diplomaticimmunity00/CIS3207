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
	_e.time = this->clock->get_ticks() + rand()%30; //Placeholder
	this->eventQueue.push(_e);
}

void Simulation::assign_control_core(int coreID) {
	this->controlCore = this->cores.at(coreID);
	debug("Assigned event handling to core "+std::to_string(coreID));
}

void Simulation::generate_components() {

	//Generate cores
	for(int i=0;i<this->numCores;i++) {
		this->cores.push_back(new CPU(i));
	}

	this->assign_control_core(0);

	//Generate disks
	for(int i=0;i<this->numDisks;i++) {
		this->disks.push_back(new Disk(i));
	}
}

int Simulation::get_control_core_id() {
	//Returns -1 if no control core is assigned
	for(int i=0;i<this->cores.size();i++) {
		if((char*)this->controlCore == (char*)this->cores.at(i)) {
			return i;
		}
	}
	return -1;
}

int Simulation::get_first_free_core() {
	//Returns -1 if all cores are occupied
	int controlCoreID = this->get_control_core_id();

	if(this->numCores == 1 and this->controlCore->is_free()) {
		return controlCoreID;
	}

	for(int i=0;i<this->numCores;i++) {
		if(this->cores.at(i)->is_free() and i != controlCoreID) {
			return i;
		}
	}

	//debug("ERROR: All cores are occupied");
	return -1;
}

int Simulation::get_best_disk() {
	DiskPriority diskPriority;
	for(int i=0;i<this->numDisks;i++) {
		diskPriority.push(*this->disks.at(i));
	}
	return diskPriority.top().id;
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

	//If job is assigned to control core, suspend ALL event handling until job is finished
	//Basically just set this job to highest possible priority
	if( (char*)this->get_core(coreID) == (char*)this->controlCore) {
		_e.time = 0;
	} else {
		//Otherwise, process can be executed in parallel with event handling
		//Which makes it possible but not guaranteed that other events will be processed before
		//Event e finishes executing
		_e.time = 0;//this->clock->get_ticks();
	}
	this->enqueue(_e);

	debug("PID "+std::to_string(_e.process->id)+" queued for CORE "+std::to_string(coreID));
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
	//From this point until arrival of PROCESS_FINISH_CPU this core will be occupied
	this->cores.at(e.process->targetCore)->receive_job(e);
	
	//Create event for process ending on cpu
	Event _e = Event(PROCESS_FINISH_CPU);
	_e.process = e.process;

	//Advancing time here presumes threading
	//Advance time here for other components
	//_e.time = this->clock->get_ticks() + rand()%30;
	
	//Set to highest possible priority
	//_e.time = 1+rand()%30;//this->clock->get_ticks()+1+rand()%30;
	//_e.time = e.time;
	_e.process->time = 1+rand()%30;
	if( (char*)this->get_core(e.process->targetCore) == (char*)this->controlCore) {
        _e.time = e.time;
    } else {
        //Otherwise, process can be executed in parallel with event handling
        //Which makes it possible that other events will be processed before
        //Event e finishes executing
        _e.time = this->clock->get_ticks()+_e.process->time;
    }
	this->enqueue(_e);

}

void Simulation::handle_cpu_exit(Event e) {

	//Simulate CPU running some program (time placeholder)
	
	this->get_core(e.process->targetCore)->finish_job();

	//Only advance clock artificially if job ran on control core
	//Otherwise time is advanced naturally by the control core
	if(this->get_control_core_id() == e.process->targetCore) {
    	this->clock->step(e.process->time);
	}

	if(rand()%10 < 3) {
		Event _e = Event(PROCESS_EXIT);
		_e.time = this->clock->get_ticks();
		_e.process = e.process;
		this->enqueue(_e);
	} else {
		Event _e = Event(PROCESS_ARRIVE_DISK);

	}

	if(!this->cpuQueue.empty()) {

		//When cpu finishes, pull next process off queue
		Event nextProcessInQueue = cpuQueue.front();
		cpuQueue.pop();

		debug("Pulling PID "+std::to_string(nextProcessInQueue.process->id)+" off of CPU queue...");

		int freecore = this->get_first_free_core();//nextProcessInQueue.process->targetCore;

		this->dispatch_job(nextProcessInQueue,freecore);
	}

}

void Simulation::handle_disk_arrival(Event e) {
		
}
void Simulation::handle_disk_exit(Event e) {

}

void Simulation::handle_system_exit(Event e) {
	debug("PID "+std::to_string(e.process->id)+" exits system");
}

void Simulation::process_from_queue() {
	if(this->eventQueue.empty()) {
		return;
	}
	Event e = this->eventQueue.top();
	this->eventQueue.pop();
	//this->clock->step(e.time);
	switch(e.type) {
		case PROCESS_ARRIVAL:
			//Process enters system
			this->handle_system_arrival(e);
			break;
		case PROCESS_EXIT:
			//Process exits system
			this->handle_system_exit(e);
			break;
		case PROCESS_ARRIVE_CPU:
			//Process arrives at a core
			this->handle_cpu_arrival(e);
			break;
		case PROCESS_FINISH_CPU:
			//Process finishes running on a core
			this->handle_cpu_exit(e);
			break;
		case PROCESS_ARRIVE_DISK:
			this->handle_disk_arrival(e);
			break;
		default:
			debug("Unhandled case for event type "+std::to_string(e.type));
	}
	//Simulate passage of time
	this->clock->step();
	//this->clock->step(e.time);
}
