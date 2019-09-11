#include <map>

#include "Simulation.h"
#include "Common.h"


std::map<ComponentType,std::string> componentMap = {
        {COMPONENT_CPU,"CORE"},
        {COMPONENT_DISK,"DISK"},
};

Component::Component(ComponentType _type, int _id): id(_id), type(_type) {
	this->free = true;
}

CPU::CPU(int _id): Component(COMPONENT_CPU,_id) {
}

void Component::receive_job(Event e) {
	this->job = e;
	sim.debug("Received PID "+std::to_string(e.process->id)+" on "+componentMap.at(this->type)+" "+std::to_string(this->id));
	this->free = false;
}

void Component::complete_job() {	
	sim.debug("Finished PID "+std::to_string(this->job.process->id)+" on "+componentMap.at(this->type)+" "+std::to_string(this->id));
	this->free = true;
}

Disk::Disk(int _id): Component(COMPONENT_DISK,_id) {
}
