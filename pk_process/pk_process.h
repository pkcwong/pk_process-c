#ifndef _PK_PROCESS
#define _PK_PROCESS

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#define MAX_QUEUE_SIZE 32

typedef struct process_t Process;
typedef struct queue_t Queue;

extern int (*process_tick)(void);
extern Queue Service;
extern void service_config(Queue* queue);
extern void batch_config(Queue* queue);

struct process_t
{
	bool kill_flag;
	int first_tick;
	int last_tick;
	void (*onExecute)(Process* this);
	void (*onDestroy)(Process* this);
	Queue* parent;
	Process** reference;
};

struct queue_t
{
	int size;
	Process process[MAX_QUEUE_SIZE];
	Process* reference[MAX_QUEUE_SIZE];
	void (*run_config)(Queue* queue);
};

void process_clock(int (*ticks)(void));
void process_init(Queue* queue, void (*run_config)(Queue* queue));

Process** process_add(Queue* queue, void (*onExecute)(Process* this), void (*onDestroy)(Process* this));
void set_kill_flag(Process* this);
void process_kill(Process* this);

void run(Queue* queue);

#endif

