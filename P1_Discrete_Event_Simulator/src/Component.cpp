#include <map>

#include "Simulation.h"
#include "Common.h"


std::map<ComponentType,std::string> componentMap = {
		{COMPONENT_CPU,"CORE"},
		{COMPONENT_DISK,"DISK"},
};

Component::Component(ComponentType _type, int _id): id(_id), type(_type) {
	this->free = true;
	this->totalUsageTime = 0;
	this->totalJobs = 0;
	this->maxResponseTime = 0;
}

CPU::CPU(int _id): Component(COMPONENT_CPU,_id) {
}

void Component::receive_job(Event e) {
	this->job = e;
	sim.debug("Received PID "+std::to_string(e.process->id)+" on "+componentMap.at(this->type)+" "+std::to_string(this->id));
	this->free = false;
}

void Disk::complete_job() {
    sim.debug("Finished PID "+std::to_string(this->job.process->id)+" on "+componentMap.at(this->type)+" "+std::to_string(this->id));
    this->totalUsageTime = this->totalUsageTime + this->job.process->ioTime;
    this->totalJobs++;
    if(this->job.process->ioTime > this->maxResponseTime) {
        this->maxResponseTime = this->job.process->ioTime;
    }
    this->free = true;
}

void CPU::complete_job() {
    sim.debug("Finished PID "+std::to_string(this->job.process->id)+" on "+componentMap.at(this->type)+" "+std::to_string(this->id));
    this->totalUsageTime = this->totalUsageTime + this->job.process->cpuTime;
    this->totalJobs++;
    if(this->job.process->cpuTime > this->maxResponseTime) {
        this->maxResponseTime = this->job.process->cpuTime;
    }
    this->free = true;
}


Disk::Disk(int _id): Component(COMPONENT_DISK,_id) {
}
