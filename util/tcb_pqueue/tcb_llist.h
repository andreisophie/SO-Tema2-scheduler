#ifndef tcb_LINKED_LIST_H_
#define tcb_LINKED_LIST_H_

#include "tcb.h"

typedef struct tcb_node_t tcb_node_t;
struct tcb_node_t {
	tcb_t *data;
	tcb_node_t *next;
};

typedef struct tcb_list_t tcb_list_t;
struct tcb_list_t {
	tcb_node_t *head;
	tcb_node_t *tail;
	unsigned int size;
};

// creates a new tcb_list_t struct
tcb_list_t *tcb_list_create(void);

// adds a new element into the list, at the end
int tcb_list_add_last(tcb_list_t *list, tcb_t *data);

// removes the first element from the list
tcb_t *tcb_list_remove_first(tcb_list_t *list);

// returns the number of elements in the list
int tcb_list_size(tcb_list_t *list);

void tcb_list_free(tcb_list_t *list);

#endif /* tcb_LINKED_LIST_H_ */
