#include <stdlib.h>

#include "tcb_llist.h"
#include "tcb.h"

tcb_list_t *tcb_list_create() {
    tcb_list_t *new = malloc(sizeof(tcb_list_t));

    new->head = NULL;
    new->tail = NULL;
    new->size = 0;

    return new;
}

int tcb_list_add_last(tcb_list_t *list, tcb_t *data) {
    if (!list) {
        return -1;
    }

    tcb_node_t *new_node = malloc(sizeof(tcb_node_t));

    new_node->data = data;
    new_node->next = NULL;

    if (list->size == 0) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = list->tail->next;
    }

    return 0;
}

tcb_t *tcb_list_remove_first(tcb_list_t *list) {
    if (!list) {
        return NULL;
    }

    if (list->size < 1) {
        return NULL;
    }

    tcb_node_t *removed_node = list->head;
    list->head = list->head->next;

    tcb_t *removed_tcb = removed_node->data;

    free(removed_tcb);

    return removed_tcb;
}

int tcb_list_size(tcb_list_t *list) {
    if (!list) {
        return -1;
    }
    return list->size;
}

void tcb_list_free(tcb_list_t *list) {
    if (!list) {
        return;
    }

    tcb_node_t *current_node = list->head;
    tcb_node_t *next_node;

    while (current_node) {
        next_node = current_node->next;
        free(current_node->data);
        free(current_node);
        current_node = next_node;
    }

    free(list);
}