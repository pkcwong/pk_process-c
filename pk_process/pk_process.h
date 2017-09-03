#ifndef _PK_PROCESS_H
#define _PK_PROCESS_H

#define MAX_QUEUE_SIZE 16

class Process;
class Queue;

class Process
{

	private:
	bool kill_flag;
	int period;
	int first_tick;
	int last_tick;
	void (*onExecute)(Process* ref);
	void (*onDestroy)(Process* ref);
	Queue* parent;
	Process** reference;

	public:
	Process(void (*onExecute)(Process* ref), void (*onDestroy)(Process* ref), int period = 0);

};

class Queue
{

	private:
	int size;
	Process process[MAX_QUEUE_SIZE];
	Process* reference[MAX_QUEUE_SIZE];

	public:
	Queue(void (*run_config)(Queue* queue));
	void add(void (*onExecute)(Process* ref), void (*onDestroy)(Process* ref));
	void run();

};

void set_kill_flag(Process* ref);
void process_kill(Process* ref);

#endif

