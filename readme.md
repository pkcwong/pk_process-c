# pk_process

release date - 2017/05/30

Log:
- added support for user-defined batch Queue
- added run configuration for batch Queue
- added support for service Queue
- added run configuration for service Queue

---

## Brief

Process management library for finite state machines. Supports basic task queueing, termination and run configuration.

## Terminology

### Process
The following shows a Process structure.
```c
struct process_t
{
	bool kill_flag;
	int first_tick;
	int last_tick;
	void (*onExecute)(struct process_t* this);
	void (*onDestroy)(struct process_t* this);
	struct queue_t* parent;
	struct process_t** reference;
};
```
Each Process is bound with ```onExecute``` and ```onDestroy``` function pointers. Function ```onExecute``` gets called once when executed by ```run()```. Function ```onDestroy``` gets called once when triggered by ```process_kill()```. Variable ```first_tick``` stores the time on first execution, while variable ```last_tick``` stores the time on last execution.

 ### Queue
 The following shows a Queue structure.
 ```c
 struct queue_t
{
	int size;
	struct process_t process[MAX_QUEUE_SIZE];
	struct process_t* reference[MAX_QUEUE_SIZE];
	void (*run_config)(struct queue_t* queue);
};
 ```
 Each Queue is bound with ```run_config``` function pointer, which is the run configuration for the Queue. Service Queue has been declared and included along with the library. Common run configuration rules, ```service_config``` and ```batch_config```, are also provided along with this library. Variable ```size``` stores the current number of Process objects in the Queue.

## Method Summary

Method Declaration | Arguments | Method Description
--- | --- | ---
void process_clock | int (\*ticks)(void) | initialize the function pointer to timer
void process_init | Queue\* queue, void (\*run_config)(Queue* queue) | initialize a Queue with run configuration
Process\*\* process_add| Queue\* queue, void (\*onExecute)(Process\* this), void (\*onDestroy)(Process\* this) | add a Process to Queue
void set_kill_flag | Process\* this | sets a Process to be terminated on next cycle
void process_kill | Process\* this | instantly terminates a Process
void run | Queue\* queue | Executes a Queue for one cycle

## Variables and Constants

Variables | Description
--- | ---
Queue Service | service Queue
void \*(service_config)(Queue* queue) | run configuration for service Queue
void \*(batch_config)(Queue* queue) | run configuration for batch Queue

## Method Details

### void process_clock

This method sets the timer function pointer for the Process library, which initializes the following variable. This is an initialization of the library and must be called on top of library usage.
```c
int (*tick)(void);
```
The clock function will be used for time logging during Process execution. Sample code as follows.
```c
#include "pk_process.h"

#include <time.h>

int timer(void)
{
	return time(NULL);
}

int main()
{
	process_clock(&timer);
}
```

### void process_init

This method initializes the Queue and binds with a run configuration. This is an initialization of the Queue and must be called on top of Queue usage. Sample code as follows.
```c
#include "pk_process.h"

#include <time.h>

int timer(void)
{
	return time(NULL);
}

int main()
{
	process_clock(&timer);
	process_init(&Service, &service_config);
}
```
To create a user-defined Queue, perform the following.
```c
#include "pk_process.h"

#include <time.h>

Queue Batch;

int timer(void)
{
	return time(NULL);
}

int main()
{
	process_clock(&timer);
	process_init(&Batch, &batch_config);
}
```
To develop a new run configuration, perform the following.
```c
/**
 * The following is a sample run configuration implementation for batch Queue logic.
 */
void myConfig(Queue* queue)
{
	if (queue->size > 0)
	{
		if (queue->process[0].kill_flag)	// provide Process termination logic
		{
			process_kill(&queue->process[0]);
		}
		else
		{
			queue->process[0].last_tick = tick();	// provide time logging
			if (queue->process[0].first_tick == 0)	// provide first execution time logging logic
			{
				queue->process[0].first_tick = queue->process[0].last_tick;
			}
			if (queue->process[0].onExecute != NULL)
			{
				queue->process[0].onExecute(&queue->process[0]);
			}
		}
	}
}
```

### Process\*\* process_add

This method adds a Process to the Queue. Sample code as follows.
```c
/**
 * The following code executes func1, printing the timestamp and message "I am executed!".
 */
#include "pk_process.h"

#include <stdio.h>
#include <time.h>

int timer(void)
{
	return time(NULL);
}

void func1(Process* this)
{
	printf("<%d> I am executed!\n", this->last_tick);
}

int main()
{
	process_clock(&timer);
	process_init(&Service, &service_config);
	Process** myProcess = process_add(&Service, &func1, NULL);
	run(&Service);
}
```

### void set_kill_flag

This method sets the Process to be terminated on next cycle. Sample code as follows.
```c
/**
 * The following code executes func1, printing the timestamp and message "I am executed!", finally terminates the Process and prints message "I am destroyed!".
 */
#include "pk_process.h"

#include <stdio.h>
#include <time.h>

int timer(void)
{
	return time(NULL);
}

void func1(Process* this)
{
	printf("<%d> I am executed!\n", this->last_tick);
}

void func2(Process* this)
{
	printf("I am destroyed!\n");
}

int main()
{
	process_clock(&timer);
	process_init(&Service, &service_config);
	Process** myProcess = process_add(&Service, &func1, &func2);
	run(&Service);
	set_kill_flag(*myProcess);
	run(&Service);
}
```

### void process_kill

This method is called and managed by the library and should not be called by user, except on developing a user-defined run configuration.

### void run

This method executes the Queue according to its run configuration. Should be called in a continuous loop.
