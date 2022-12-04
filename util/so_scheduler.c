#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "so_scheduler.h"
#include "tcb_pqueue/tcb.h"
#include "tcb_pqueue/tcb_pqueue.h"

static unsigned int timeq;
static unsigned int io_count;
static unsigned int initialized = 0;
static unsigned int useless = 0;
static tcb_pqueue_t *ready;
static tcb_t *running;

DECL_PREFIX int so_init(unsigned int time_quantum, unsigned int io) {
    if (io > SO_MAX_NUM_EVENTS || time_quantum < 1 || initialized == 1) {
        return -1;
    }
    timeq = time_quantum;
    io_count = io;
    initialized = 1;

    ready = tcb_pqueue_init();
    running = NULL;

    return 0;
}

DECL_PREFIX tid_t so_fork(so_handler *func, unsigned int priority) {
    if (!func || priority > SO_MAX_PRIO) {
        return INVALID_TID;
    }

    tcb_t *new_thread = malloc(sizeof(tcb_t));
    new_thread->prio = priority;

    pthread_create(&(new_thread->tid), NULL, (void *)func, &priority);

    pthread_join(new_thread->tid, NULL);

    free(new_thread);

    return new_thread->tid;
}

DECL_PREFIX int so_wait(unsigned int io) {

}

DECL_PREFIX int so_signal(unsigned int io) {

}

DECL_PREFIX void so_exec(void) {
    useless++;
}

DECL_PREFIX void so_end(void) {
    tcb_pqueue_free(ready);
    initialized = 0;
}