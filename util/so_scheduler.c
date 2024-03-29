#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#include "so_scheduler.h"
#include "tcb_pqueue/tcb.h"
#include "tcb_pqueue/tcb_pqueue.h"

#define CAPACITY 10000

static unsigned int timeq;
static unsigned int io_count;
static unsigned int initialized;
static tcb_pqueue_t *pqueue;
static unsigned int total_threads;
static tcb_t *all[CAPACITY];
static tcb_t *running;
static tcb_list_t *waiting;

void schedule(void)
{
	// if there is no thread running
	if (!running) {
		// just run next thread in queue
		running = tcb_pqueue_dequeue(pqueue);
		running->state = RUNNING;
		running->time = timeq;
		sem_post(&(running->sem));
		return;
	}
	// if there is no thread waiting in queue
	if (tcb_pqueue_size(pqueue) == 0) {
		// continue running last thread
		sem_post(&(running->sem));
		return;
	}
	// get next thread that is waiting
	tcb_t *next = tcb_pqueue_peek(pqueue);

	// check if current thread can continue running
	if (running->state >= WAITING) {
		// if not, run next thread
		running = tcb_pqueue_dequeue(pqueue);
		running->state = RUNNING;
		running->time = timeq;
		sem_post(&(running->sem));
		return;
	}
	// if yes, compare its priority with next waiting thread
	if (running->prio < next->prio) {
		// if curret thread has lower prio, run next thread
		running->state = READY;
		tcb_pqueue_enqueue(pqueue, running, running->prio);
		running = tcb_pqueue_dequeue(pqueue);
		running->state = RUNNING;
		running->time = timeq;
		sem_post(&(running->sem));
		return;
	}
	// check if running is out of time
	if (running->time == 0) {
		//check if running and waiting have equal priorities
		if (running->prio == next->prio) {
			// if yes, swap the threads
			running->state = READY;
			tcb_pqueue_enqueue(pqueue, running, running->prio);
			running = tcb_pqueue_dequeue(pqueue);
			running->state = RUNNING;
			running->time = timeq;
			sem_post(&(running->sem));
			return;
		}
		// if not, give running thread another quantum
		running->time = timeq;
	}

	sem_post(&(running->sem));
}

void wrapper_start_thread(void *param)
{
	tcb_t *this = (tcb_t *)param;
	// wait for thread to be scheduled
	sem_wait(&(this->sem));
	// run handler
	this->func(this->prio);
	// terminate thread
	this->state = TERMINATED;
	schedule();
}

DECL_PREFIX int so_init(unsigned int time_quantum, unsigned int io)
{
	if (io > SO_MAX_NUM_EVENTS || time_quantum < 1 || initialized == 1)
		return -1;

	timeq = time_quantum;
	io_count = io;
	initialized = 1;

	pqueue = tcb_pqueue_init();
	waiting = tcb_list_create();
	running = NULL;

	return 0;
}

DECL_PREFIX tid_t so_fork(so_handler *func, unsigned int priority)
{
	if (!func || priority > SO_MAX_PRIO)
		return INVALID_TID;

	// remembers if a fork has been done before
	// initialized with 0
	static int forked;

	tcb_t *new_thread = malloc(sizeof(tcb_t));

	new_thread->prio = priority;
	new_thread->func = func;
	new_thread->state = NEW;
	new_thread->time = timeq;
	sem_init(&(new_thread->sem), 0, 0);

	pthread_create(&(new_thread->tid), NULL, (void *)wrapper_start_thread, new_thread);
	all[total_threads++] = new_thread;

	tcb_pqueue_enqueue(pqueue, new_thread, new_thread->prio);

	tcb_t *current_running = running;

	if (forked)
		current_running->time--;

	schedule();

	if (forked)
		sem_wait(&(current_running->sem));

	forked = 1;

	return new_thread->tid;
}

DECL_PREFIX int so_wait(unsigned int io)
{
	if (io >= io_count || io < 0)
		return -1;

	tcb_t *current_running = running;

	running->state = WAITING;
	running->io_index = io;

	tcb_list_add_last(waiting, running);

	schedule();

	sem_wait(&(current_running->sem));

	return 0;
}

DECL_PREFIX int so_signal(unsigned int io)
{
	if (io >= io_count || io < 0)
		return -1;

	unsigned int nr_waiting = tcb_list_size(waiting);
	unsigned int nr_woken = 0;
	tcb_t *waiting_thread;

	for (int i = 0; i < nr_waiting; i++) {
		waiting_thread = tcb_list_remove_first(waiting);
		if (waiting_thread->io_index == io) {
			waiting_thread->state = READY;
			tcb_pqueue_enqueue(pqueue, waiting_thread, waiting_thread->prio);
			nr_woken++;
		} else {
			tcb_list_add_last(waiting, waiting_thread);
		}
	}

	running->time--;

	tcb_t *current_running = running;

	schedule();

	sem_wait(&(current_running->sem));

	return nr_woken;
}

DECL_PREFIX void so_exec(void)
{
	tcb_t *current_running = running;

	running->time--;

	schedule();

	sem_wait(&(current_running->sem));
}

DECL_PREFIX void so_end(void)
{
	if (initialized) {
		for (int i = 0; i < total_threads; i++) {
			pthread_join(all[i]->tid, NULL);
			sem_destroy(&(all[i]->sem));
			free(all[i]);
		}
		// free priority queue
		tcb_pqueue_free(pqueue);
		tcb_list_free(waiting);
		initialized = 0;
	}
}
