#include "Common.h"

#include <ctime>

Event::Event(EventType _type): type(_type)  {
	this->process = new Process();
}

Process::Process() {
	this->id = ++sim.pids;
}
