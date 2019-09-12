#include "Simulation.h"
#include "Clock.h"
#include <chrono>
#include <thread>

Simulation::Simulation() {
	this->config->import_config_from_file("config.txt");
	this->numCores = this->config->get_config_value(CORES);
	this->numDisks = this->config->get_config_value(DISKS);

	this->clock->set_time(this->config->get_config_value(INIT_TIME));

	this->generate_components();
    this->running = true;
}

Simulation::Simulation(int cores,int disks): numCores(cores), numDisks(disks) {
    this->config->import_config_from_file("config.txt");

	this->clock->set_time(this->config->get_config_value(INIT_TIME));

    this->generate_components();
    this->running = true;
}

void Simulation::enqueue(Event e) {
	this->eventQueue.push(e);
}
void Simulation::enqueue(EventType e) {
	Event _e = Event(e);
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
	//Sets best_queue to id of disk with smallest queue
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
	debug("Simulation starting with "+std::to_string(this->numCores)+" cores and "+std::to_string(this->numDisks)+" disks",false);
}

void Simulation::dispatch_job(Event e,ComponentType type,int id) {
	//Creates a new event for process arrival at CPU

	switch(type) {
		case COMPONENT_CPU: {
			//Prevent core from scheduling subsequent jobs	
			this->get_core(id)->lock();
		
			//Create event for process arriving to cpu
			Event _e = Event(PROCESS_ARRIVE_CPU);
			_e.process = e.process;
			_e.process->targetComponentID = id;
		
			//If job is assigned to control core, suspend ALL event handling until job is finished
			//Basically just set this job to highest possible priority
			if( id == this->get_control_core_id()) {
				_e.time = 0;
			} else {
				//Otherwise, process can be executed in parallel with event handling
				//Which makes it possible but not guaranteed that other events will be processed before
				//Event e finishes executing
				_e.time = this->clock->get_ticks();
			}
			this->enqueue(_e);
		
			debug("Sent PID "+std::to_string(_e.process->id)+" to CORE "+std::to_string(id));
			break;
		}
		case COMPONENT_DISK: {
			//Create disk arrival event
			//Lock disk to prevent process conflicts
			this->get_disk(id)->lock();
			Event _e = Event(PROCESS_ARRIVE_DISK);
			_e.process = e.process;
			_e.process->targetComponentID = id;
			_e.time = 0;//this->clock->get_ticks();
			//Basically just set this job to highest possible priority
			this->enqueue(_e);
			debug("Sent PID "+std::to_string(_e.process->id)+" to DISK "+std::to_string(id));
			break;
		}
		
	}
}

void Simulation::handle_system_arrival(Event e) {
	//e.time = this->clock->get_ticks();

	debug("PID "+std::to_string(e.process->id)+" enters system");

	//Check queues and cores
	int freecore = this->get_first_free_core();
	if(freecore >= 0 and this->cpuQueue.empty()) {
		
		this->dispatch_job(e,COMPONENT_CPU,freecore);

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
	this->cores.at(e.process->targetComponentID)->receive_job(e);
	
	//Create event for process ending on cpu
	Event _e = Event(PROCESS_FINISH_CPU);
	_e.process = e.process;

	_e.process->cpuTime = this->config->get_range(CPU_MIN,CPU_MAX);
	if(e.process->targetComponentID == this->get_control_core_id()) {
		//If process is running on control core
		//Send it to CPU immediately
		_e.time = e.time;
	} else {
		//Otherwise, process can be executed in parallel with event handling
		//Which makes it possible that other events will be processed before
		//Event e finishes executing
		_e.time = this->clock->get_ticks()+_e.process->cpuTime;
	}
	this->enqueue(_e);

}

void Simulation::handle_cpu_exit(Event e) {

	//Simulate CPU running some program (time placeholder)
	//Only advance clock artificially if job ran on control core
    //Otherwise time is advanced naturally by the control core
    if(this->get_control_core_id() == e.process->targetComponentID) {
        this->clock->step(e.process->cpuTime);
    }

	this->get_core(e.process->targetComponentID)->complete_job();

	Event _e;

	if(rand()%100 < this->config->get_config_value(QUIT_PROB)) {
		//Set process to system exit
		_e = Event(PROCESS_EXIT);
		_e.time = this->clock->get_ticks();
   		_e.process = e.process;
   		this->enqueue(_e);
	} else {
		//Set process to begin IO
		//IO time is assigned in handle_arrive_disk
    	int bestDisk = this->get_first_free_disk();
		if(bestDisk >= 0) {
    		this->dispatch_job(e,COMPONENT_DISK,bestDisk);
		} else {
			debug("Pushed PID "+std::to_string(e.process->id)+" to DISK "+std::to_string(this->get_disk(this->get_best_disk_queue())->id)+" queue");
			this->get_disk(this->get_best_disk_queue())->queue.push(e);
		}
	}

	if(!this->cpuQueue.empty()) {

		//When cpu finishes, pull next process off queue
		Event nextProcessInQueue = cpuQueue.front();
		cpuQueue.pop();

		debug("Pulling PID "+std::to_string(nextProcessInQueue.process->id)+" off of CPU queue...");

		int freecore = this->get_first_free_core();//nextProcessInQueue.process->targetComponentID;

		this->dispatch_job(nextProcessInQueue,COMPONENT_CPU,freecore);
	}

}

void Simulation::handle_disk_arrival(Event e) {
	//Create disk finish event
	//Execution does not wait for IO to finish
	this->get_disk(e.process->targetComponentID)->receive_job(e);

	Event _e = Event(PROCESS_FINISH_DISK);
	_e.process = e.process;
	_e.process->ioTime = this->config->get_range(DISK_MIN,DISK_MAX); //Placeholder
	_e.time = this->clock->get_ticks() + _e.process->ioTime;
	this->enqueue(_e);
}
void Simulation::handle_disk_exit(Event e) {
	this->get_disk(e.process->targetComponentID)->complete_job();

	Event _e = Event(PROCESS_ARRIVAL);
	_e.time = e.time;
	_e.process = e.process;
	
	this->enqueue(_e);

	if(!this->get_disk(e.process->targetComponentID)->queue.empty()) {
		Event nextProcessInQueue = this->get_disk(e.process->targetComponentID)->queue.front();
		this->get_disk(e.process->targetComponentID)->queue.pop();

		debug("Pulling PID "+std::to_string(nextProcessInQueue.process->id)+" off of DISK "+std::to_string(e.process->targetComponentID)+" queue...");
		this->dispatch_job(nextProcessInQueue,COMPONENT_DISK,e.process->targetComponentID);
	}
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
	//this->clock->step();
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
		case PROCESS_FINISH_DISK:
			this->handle_disk_exit(e);
			break;
		default:
			debug("Unhandled case for event type "+std::to_string(e.type));
	}
	//Simulate passage of time
	this->clock->step();
	//this->clock->step(e.time);
}
