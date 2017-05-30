#ifndef _PK_PROCESS
#define _PK_PROCESS

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#define MAX_QUEUE_SIZE 32

typedef struct process_t Process;
typedef struct queue_t Queue;

extern int (*tick)(void);
extern const struct queue_t Service;
extern void service_config(struct queue_t* queue);
extern void batch_config(struct queue_t* queue);

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

struct queue_t
{
	int size;
	struct process_t process[MAX_QUEUE_SIZE];
	struct process_t* reference[MAX_QUEUE_SIZE];
	void (*run_config)(struct queue_t* queue);
};

void process_clock(int (*ticks)(void));
void process_init(struct queue_t* queue, void (*run_config)(struct queue_t* queue));

struct process_t** process_add(struct queue_t* queue, void (*onExecute)(struct process_t* this), void (*onDestroy)(struct process_t* this));
void set_kill_flag(struct process_t* this);
void process_kill(struct process_t* this);

void run(struct queue_t* queue);

#endif

