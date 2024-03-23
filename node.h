#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum TYPE {INT, FLOAT, CHAR, STRING, REFERENCE};

struct node {
    void* val; //Pointer to the value which the node stores
    struct node* next; //Pointer to the next node in the node
    enum TYPE* type; //Type of value for type casting
};

extern void node_check();

extern struct node *node_create(void* value, enum TYPE* type);

extern void node_free(struct node * n);

//Must free the returned char after
extern void node_val(struct node * n, char * ret);

//Must free the returned char after
extern void node_type(struct node * n, char * ret);



#endif
