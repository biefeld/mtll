#ifndef MTLL_H
#define MTLL_H

#include "node.h"
#include <stddef.h>

struct mtll {
    struct node* head; //Pointer to the head of the list
    struct mtll* next; //Pointer to the next mtll
    size_t index; //Index of this mtll
    // int size; //Number of elements in the list
    int has_nested; //If this mtll has references to other mtll
    int num_references; //Number of times this mtll has been refrenced
};

extern void mtll_check();

extern struct mtll *mtll_create(size_t * num_nodes, size_t* next_index, void** values, enum TYPE* types);

extern void mtll_free(struct mtll * m);

extern void mtll_free_all(struct mtll * head);

extern void mtll_view(struct mtll * m);

extern void mtll_type(struct mtll * m);

extern void mtll_view_all(struct mtll * head);

extern void mtll_remove(struct mtll * head, size_t n);

extern void mtll_insert();

extern void mtll_delete();


#endif
