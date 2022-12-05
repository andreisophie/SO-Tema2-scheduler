#ifndef tcb_PRIORITY_QUEUE_H_
#define tcb_PRIORITY_QUEUE_H_

#include "tcb.h"
#include "tcb_llist.h"
#include "../so_scheduler.h"

#define PQUEUE_CAPACITY (SO_MAX_PRIO + 1)

typedef struct {
	tcb_list_t *lists[PQUEUE_CAPACITY];
	unsigned int max_prio;
} tcb_pqueue_t;

tcb_pqueue_t *tcb_pqueue_init(void);

int tcb_pqueue_enqueue(tcb_pqueue_t *pqueue, tcb_t *data, unsigned int prio);

tcb_t *tcb_pqueue_peek(tcb_pqueue_t *pqueue);

tcb_t *tcb_pqueue_dequeue(tcb_pqueue_t *pqueue);

int tcb_pqueue_size(tcb_pqueue_t *pqueue);

void tcb_pqueue_free(tcb_pqueue_t *pqueue);

#endif /* tcb_PRIORITY_QUEUE_H_ */
