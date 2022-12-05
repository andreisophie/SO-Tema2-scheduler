#include <stdlib.h>
#include <stdio.h>

#include "tcb_pqueue.h"
#include "tcb_llist.h"
#include "tcb.h"

tcb_pqueue_t *tcb_pqueue_init(void)
{
	tcb_pqueue_t *new = malloc(sizeof(tcb_pqueue_t));

	new->max_prio = SO_MAX_PRIO;

	for (unsigned int i = 0; i <= new->max_prio; i++)
		new->lists[i] = tcb_list_create();

	return new;
}

int tcb_pqueue_enqueue(tcb_pqueue_t *pqueue, tcb_t *data, unsigned int prio)
{
	if (!pqueue || prio > pqueue->max_prio)
		return -1;

	tcb_list_add_last(pqueue->lists[prio], data);

	return 0;
}

tcb_t *tcb_pqueue_peek(tcb_pqueue_t *pqueue)
{
	if (!pqueue)
		return NULL;

	for (int i = pqueue->max_prio; i >= 0 ; i--)
		if (tcb_list_size(pqueue->lists[i]) > 0)
			return pqueue->lists[i]->head->data;

	return NULL;
}

tcb_t *tcb_pqueue_dequeue(tcb_pqueue_t *pqueue)
{
	if (!pqueue)
		return NULL;

	for (int i = pqueue->max_prio; i >= 0 ; i--)
		if (tcb_list_size(pqueue->lists[i]) > 0)
			return tcb_list_remove_first(pqueue->lists[i]);

	return NULL;
}

int tcb_pqueue_size(tcb_pqueue_t *pqueue)
{
	if (!pqueue)
		return -1;

	unsigned int total_size = 0;

	for (int i = 0; i <= pqueue->max_prio; i++)
		total_size += tcb_list_size(pqueue->lists[i]);

	return total_size;
}

void tcb_pqueue_free(tcb_pqueue_t *pqueue)
{
	if (!pqueue)
		return;

	for (int i = 0; i <= pqueue->max_prio; i++)
		tcb_list_free(pqueue->lists[i]);

	free(pqueue);
}
