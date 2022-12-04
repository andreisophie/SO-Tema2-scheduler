#ifndef tcb_H_
#define tcb_H_

#include <pthread.h>
#include <semaphore.h>
#include "../so_scheduler.h"

// thread states
#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define TERMINATED 4

typedef struct tcb_t tcb_t;
struct tcb_t {
    tid_t tid; // thread ID returned by pthread_create(...)
    unsigned int prio; // priority of thread
    so_handler *func; // function that the thread runs
	unsigned int state; // state of thread -> value 0..4
    unsigned int time; // how much time is left for this thread
    sem_t run; // semaphore which tells this thread to wait or not
};

#endif /* tcb_H_ */