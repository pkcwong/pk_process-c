#include "pk_process.h"

int (*process_tick)(void);
Queue Service;
void service_config(Queue* queue);
void batch_config(Queue* queue);

void process_clock(int (*ticks)(void))
{
	process_tick = ticks;
}

void process_init(Queue* queue, void (*run_config)(Queue* queue))
{
	queue->size = 0;
	queue->run_config = run_config;
}

Process** process_add(Queue* queue, void (*onExecute)(Process* this), void (*onDestroy)(Process* this))
{
	if (queue->size != MAX_QUEUE_SIZE)
	{
		queue->process[queue->size].kill_flag = false;
		queue->process[queue->size].first_tick = 0;
		queue->process[queue->size].last_tick = 0;
		queue->process[queue->size].onExecute = onExecute;
		queue->process[queue->size].onDestroy = onDestroy;
		queue->process[queue->size].parent = queue;
		for (int i = 0; i != MAX_QUEUE_SIZE; i++)
		{
			if (queue->reference[i] == NULL)
			{
				queue->reference[i] = &queue->process[queue->size];
				queue->process[queue->size].reference = &queue->reference[i];
				queue->size++;
				return &queue->reference[i];
			}
		}
	}
	return NULL;
}

void set_kill_flag(Process* this)
{
	this->kill_flag = true;
}

void process_kill(Process* this)
{
	if (this->onDestroy != NULL)
	{
		this->onDestroy(this);
	}
	*(this->reference) = NULL;
	for (int i = 0; i != this->parent->size; i++)
	{
		if (&this->parent->process[i] == this)
		{
			for (int j = i; j != this->parent->size - 1; j++)
			{
				this->parent->process[j] = this->parent->process[j + 1];
				*(this->parent->process[j].reference) = &this->parent->process[j];
			}
			this->parent->size--;
			return;
		}
	}
}

void run(Queue* queue)
{
	queue->run_config(queue);
}

void service_config(Queue* queue)
{
	for (int i = 0; i != queue->size; i++)
	{
		if (queue->process[i].kill_flag)
		{
			process_kill(&queue->process[i]);
			i--;
		}
		else
		{
			queue->process[i].last_tick = process_tick();
			if (queue->process[i].first_tick == 0)
			{
				queue->process[i].first_tick = queue->process[i].last_tick;
			}
			if (queue->process[i].onExecute != NULL)
			{
				queue->process[i].onExecute(&queue->process[i]);
			}
		}
	}
}

void batch_config(Queue* queue)
{
	if (queue->size > 0)
	{
		if (queue->process[0].kill_flag)
		{
			process_kill(&queue->process[0]);
		}
		else
		{
			queue->process[0].last_tick = process_tick();
			if (queue->process[0].first_tick == 0)
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

