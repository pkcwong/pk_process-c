#ifndef _PK_PROCESS_H
#define _PK_PROCESS_H

#define MAX_QUEUE_SIZE 8

class Process;
class Queue;

class Process
{

	public:

	bool kill_flag;
	int period;
	int first_tick;
	int last_tick;
	void (*onExecute)(Process* ref);
	void (*onDestroy)(Process* ref);
	Queue* parent;
	Process** reference;

	Process(void (*onExecute)(Process* ref) = 0, void (*onDestroy)(Process* ref) = 0, int period = 0);
	void set_kill_flag();
	void kill();

};

class Queue
{

	public:

	int size;
	Process process[MAX_QUEUE_SIZE];
	Process* reference[MAX_QUEUE_SIZE];
	void (*run_config)(Queue* queue);

	Queue(void (*run_config)(Queue* queue));
	Process* add(void (*onExecute)(Process* ref) = 0, void (*onDestroy)(Process* ref) = 0);
	void run();

};

extern Queue Service;
extern void service_config(Queue* queue);

#endif

