#include "pk_process.h"

int (*_clock_)(void);
Queue Service(&service_config);

void process_clock(int (*tick)(void))
{
	_clock_ = tick;
}

Process::Process(void (*onExecute)(Process* ref), void (*onDestroy)(Process* ref), int period)
{
	this->onExecute = onExecute;
	this->onDestroy = onDestroy;
	this->period = period;
	this->kill_flag = false;
	this->first_tick = 0;
	this->last_tick = 0;
	this->parent = 0;
	this->reference = 0;
}

void Process::set_kill_flag()
{
	this->kill_flag = true;
}

void Process::kill()
{
	for (Process** ptr = this->reference + 1; ptr != &(this->parent->reference[this->parent->size]); ptr++)
	{
		*(ptr - 1) = *ptr;
		(*(ptr - 1))->reference = ptr - 1;
	}
	this->parent->size--;
	this->parent->reference[this->parent->size] = 0;
	this->parent = 0;
	this->reference = 0;
}

Queue::Queue(void (*run_config)(Queue* queue))
{
	this->run_config = run_config;
	this->size = 0;
}

Process* Queue::add(void (*onExecute)(Process* ref), void (*onDestroy)(Process* ref), int period)
{
	if (this->size != MAX_QUEUE_SIZE)
	{
		for (int i = 0; i != MAX_QUEUE_SIZE; i++)
		{
			if (this->process[i].reference == 0)
			{
				this->reference[this->size] = &(this->process[i]);
				this->process[i].onExecute = onExecute;
				this->process[i].onDestroy = onDestroy;
				this->process[i].period = period;
				this->process[i].kill_flag = false;
				this->process[i].first_tick = 0;
				this->process[i].last_tick = 0;
				this->process[i].parent = this;
				this->process[i].reference = &(this->reference[this->size]);
				this->size++;
				return &(this->process[i]);
			}
		}
	}
	return 0;
}

void Queue::run()
{
	if (this->run_config != 0)
	{
		this->run_config(this);
	}
}

void service_config(Queue* queue)
{
	for (int i = 0; i != queue->size; i++)
	{
		if (queue->reference[i]->kill_flag)
		{
			if (queue->reference[i]->onDestroy != 0)
			{
				queue->reference[i]->onDestroy(queue->reference[i]);
			}
			queue->reference[i]->kill();
			i--;
		}
		else
		{
			if (_clock_ == 0 || queue->reference[i]->period == 0 || _clock_() - queue->reference[i]->last_tick > queue->reference[i]->period)
			{
				if (_clock_ != 0)
				{
					queue->reference[i]->last_tick = _clock_();
				}
				if (queue->reference[i]->first_tick == 0)
				{
					queue->reference[i]->first_tick = queue->reference[i]->last_tick;
				}
				if (queue->reference[i]->onExecute != 0)
				{
					queue->reference[i]->onExecute(queue->reference[i]);
				}
			}
		}
	}
}

