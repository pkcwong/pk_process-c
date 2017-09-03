#include "pk_process.h"

Process::Process(void (*onExecute)(Process* ref), void (*onDestroy)(Process* ref), int period)
{
	this->onExecute = onExecute;
	this->onDestroy = onDestroy;
	this->period = period;
}

