#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "structures.h"


extern struct node *node_create(void* value, enum TYPE* type);

extern void node_free(struct node* n);

extern void node_val(struct node* n, char* ret);

extern void node_type(struct node* n, char* ret);

#endif
