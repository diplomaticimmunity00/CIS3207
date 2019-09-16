#include "Simulation.h"
#include "Clock.h"

#include <random>
#include <chrono>
#include <thread>

Simulation::Simulation() {
	this->config->import_config_from_file("config.ini");
	this->numCores = this->config->get_config_value(CORES);
	this->numDisks = this->config->get_config_value(DISKS);

	this->clock->set_time(this->config->get_config_value(INIT_TIME));

	this->generate_components();
	this->running = true;
}

void Simulation::enqueue(Event e) {
	this->eventQueue.push(e);
	this->queueAccesses++;
    this->totalEventsSeen++;
    if(this->eventQueue.size() > this->maxQueueSize) {
        this->maxQueueSize = this->eventQueue.size();
    }
}

void Simulation::CPUenqueue(Event e) {
	this->CPUqueueAccesses++;
	this->cpuQueue.push(e);
	this->CPUtotalEventsSeen++;
	if(this->cpuQueue.size() > this->CPUmaxQueueSize) {
		this->CPUmaxQueueSize = this->cpuQueue.size();
	}
}

void Simulation::dequeue() {
	this->queueAccesses++;
	this->eventQueue.pop();
}
void Simulation::CPUdequeue() {
    this->CPUqueueAccesses++;
    this->cpuQueue.pop();
}

void Simulation::enqueue(EventType e) {
	Event _e = Event(e);
    this->queueAccesses++;
    this->totalEventsSeen++;
    if(this->eventQueue.size() > this->maxQueueSize) {
        this->maxQueueSize = this->eventQueue.size();
    }
	_e.time = this->clock->get_ticks() + this->config->get_range(ARRIVE_MIN,ARRIVE_MAX); //Placeholder
	this->eventQueue.push(_e);
}

void Simulation::assign_control_core(int coreID) {
	this->controlCore = this->cores.at(coreID);
	debug("Assigned event handling to core "+std::to_string(coreID),false);
}

void Simulation::generate_components() {

	if(!(this->numCores) or !(this->numDisks)) {
		debug("Critical component configuration error, exiting",false);
		debug("Cores: "+std::to_string(numCores),false);
		debug("Disks: "+std::to_string(numDisks),false);
		exit(0);
	}

	//generate cores
	for(int i=0;i<this->numCores;i++) {
		this->cores.push_back(new CPU(i));
	}

	this->assign_control_core(0);

	//generate disks
	for(int i=0;i<this->numDisks;i++) {
		this->disks.push_back(new Disk(i));
	}
}

int Simulation::get_control_core_id() {
	//returns -1 if no control core is assigned
	for(int i=0;i<this->cores.size();i++) {
		if((char*)this->controlCore == (char*)this->cores.at(i)) {
			return i;
		}
	}
	return -1;
}

int Simulation::get_first_free_core() {
	//returns -1 if all cores are occupied
	int controlCoreID = this->get_control_core_id();

	for(int i=0;i<this->numCores;i++) {
		if(this->get_core(i)->is_free()) {
			return i;
		}
	}

	return -1;
}

int Simulation::get_first_free_disk() { 
	//Returns -1 if all disks are occupied
	for(int i=0;i<this->numDisks;i++) {
		if(this->get_disk(i)->is_free()) {
			return i;
		}
	}
	return -1;
}

int Simulation::get_best_disk_queue() {
	//chooses disk with smallest queue
	DiskPriority diskPriority;
	for(int i=0;i<this->numDisks;i++) {
		diskPriority.push(*this->disks.at(i));
	}
	return diskPriority.top().id;
}

void Simulation::start() {
	if(this->config->get_config_value(REALTIME)) {
		this->clock->realtime = true;
		this->clock->tickrate = this->config->get_config_value(TICKRATE);
	}
	srand(this->config->get_config_value(SEED));

	//moved to stat struct
	//debug("Simulation starting with "+std::to_string(this->numCores)+" cores and "+std::to_string(this->numDisks)+" disks",false);
	
	debug("Simulation starting");
}

void Simulation::dispatch_job(Event e,ComponentType type,int id) {
	
	//creates a new event for process arrival at a component

	switch(type) {
		case COMPONENT_CPU: {
			//prevent core from scheduling subsequent jobs	
			this->get_core(id)->lock();
		
			//create event for process arriving to cpu
			Event _e = Event(PROCESS_ARRIVE_CPU);
			_e.process = e.process;
			_e.process->targetComponentID = id;
		
			//if job is assigned to control core, suspend ALL event handling until job is finished
			//basically just set this job to highest possible priority
			if( id == this->get_control_core_id()) {
				_e.time = this->clock->get_ticks();
			} else {
				//otherwise, process can be executed in parallel with event handling
				//which makes it possible but not guaranteed that other events will be processed before
				//event e finishes executing
				_e.time = this->clock->get_ticks();
			}
			this->enqueue(_e);
		
			debug("Sent PID "+std::to_string(_e.process->id)+" to CORE "+std::to_string(id));
			break;
		}
		case COMPONENT_DISK: {
			//create disk arrival event
			//lock disk to prevent process conflicts
			this->get_disk(id)->lock();
			Event _e = Event(PROCESS_ARRIVE_DISK);
			_e.process = e.process;
			_e.process->targetComponentID = id;
			_e.time = 0;
			//basically just set this job to highest possible priority
			this->enqueue(_e);
			debug("Sent PID "+std::to_string(_e.process->id)+" to DISK "+std::to_string(id));
			break;
		}
		
	}
}

void Simulation::handle_system_arrival(Event e) {

	debug("PID "+std::to_string(e.process->id)+" enters system");

	//check queues and cores
	int freecore = this->get_first_free_core();
	if(freecore >= 0 and this->cpuQueue.empty()) {
		this->dispatch_job(e,COMPONENT_CPU,freecore);
	} else {
		debug("Pushed PID "+std::to_string(e.process->id)+" to CPU queue");
		this->CPUenqueue(e);
	}

	//create new arrival event
	this->enqueue(PROCESS_ARRIVAL);

}

void Simulation::handle_cpu_arrival(Event e) {

	//send job to target core 
	//target core ensured free as it was locked when this process was queued for arrival
	//from this point until arrival of PROCESS_FINISH_CPU this core will be occupied
	this->cores.at(e.process->targetComponentID)->receive_job(e);
	
	//create event for process finishing on cpu
	Event _e = Event(PROCESS_FINISH_CPU);
	_e.process = e.process;

	//generate process CPU time
	_e.process->cpuTime = this->config->get_range(CPU_MIN,CPU_MAX);
	_e.time = this->clock->get_ticks()+_e.process->cpuTime;
	this->enqueue(_e);

}

void Simulation::handle_cpu_exit(Event e) {

	//simulate CPU running some program (time placeholder)
	//advance time based on process CPU time
	if(this->numCores == 1) {
		this->clock->step(e.process->cpuTime);
	}

	this->get_core(e.process->targetComponentID)->complete_job();

	Event _e;

	if(rand()%100 < this->config->get_config_value(QUIT_PROB)) {
		//create system exit event
		_e = Event(PROCESS_EXIT);
		_e.time = this->clock->get_ticks();
		_e.process = e.process;
		this->enqueue(_e);
	} else {
		//set process to begin IO
		//IO time is assigned in handle_arrive_disk
		int bestDisk = this->get_first_free_disk();
		if(bestDisk >= 0) {
			this->dispatch_job(e,COMPONENT_DISK,bestDisk);
		} else {
			debug("Pushed PID "+std::to_string(e.process->id)+" to DISK "+std::to_string(this->get_disk(this->get_best_disk_queue())->id)+" queue");
			this->get_disk(this->get_best_disk_queue())->enqueue(e);
		}
	}

	if(!this->cpuQueue.empty()) {

		//when cpu finishes, pull next process off queue
		Event nextProcessInQueue = cpuQueue.front();
		this->CPUdequeue();

		debug("Pulling PID "+std::to_string(nextProcessInQueue.process->id)+" off of CPU queue...");

		int freecore = this->get_first_free_core();

		this->dispatch_job(nextProcessInQueue,COMPONENT_CPU,freecore);
	}

}

void Simulation::handle_disk_arrival(Event e) {
	//create disk finish event
	//execution does not wait for IO to finish
	this->get_disk(e.process->targetComponentID)->receive_job(e);

	Event _e = Event(PROCESS_FINISH_DISK);
	_e.process = e.process;
	_e.process->ioTime = this->config->get_range(DISK_MIN,DISK_MAX); //Placeholder
	_e.time = this->clock->get_ticks() + _e.process->ioTime;
	this->enqueue(_e);
}
void Simulation::handle_disk_exit(Event e) {
	this->get_disk(e.process->targetComponentID)->complete_job();

	//do not advance time here
	//execution does not wait for IO

	//send process back to CPU
	//must be sent as a PROCESS_ARRIVAL event
	//so that proper core/disk can be assigned
	Event _e = Event(PROCESS_ARRIVAL);
	_e.time = this->clock->get_ticks();
	_e.process = e.process;
	this->enqueue(_e);

	//if disk queue is empty
	//pop next process and dispatch to disk
	if(!this->get_disk(e.process->targetComponentID)->queue.empty()) {
		Event nextProcessInQueue = this->get_disk(e.process->targetComponentID)->queue.front();
		this->get_disk(e.process->targetComponentID)->dequeue();
		debug("Pulling PID "+std::to_string(nextProcessInQueue.process->id)+" off of DISK "+std::to_string(e.process->targetComponentID)+" queue...");
		this->dispatch_job(nextProcessInQueue,COMPONENT_DISK,e.process->targetComponentID);
	}
}

void Simulation::handle_system_exit(Event e) {
	debug("PID "+std::to_string(e.process->id)+" exits system");
	this->pidsCompleted++;
}

void Simulation::process_from_queue() {
	if(this->eventQueue.empty()) {
		return;
	}
	Event e = this->eventQueue.top();
	//pop wrapper function
	this->dequeue();
	switch(e.type) {
		case PROCESS_ARRIVAL:
			//process enters system
			this->handle_system_arrival(e);
			break;
		case PROCESS_EXIT:
			//process exits system
			this->handle_system_exit(e);
			break;
		case PROCESS_ARRIVE_CPU:
			//process arrives at a core
			this->handle_cpu_arrival(e);
			break;
		case PROCESS_FINISH_CPU:
			//process finishes running on a core
			this->handle_cpu_exit(e);
			break;
		case PROCESS_ARRIVE_DISK:
			//process arrives at disk
			this->handle_disk_arrival(e);
			break;
		case PROCESS_FINISH_DISK:
			//process finishes io
			this->handle_disk_exit(e);
			break;
		default:
			debug("Unhandled case for event type "+std::to_string(e.type));
	}
	if(this->numCores > 1) this->clock->step();
}
