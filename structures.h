#ifndef TYPE_H
#define TYPE_H

#define BUFFER (129) //Size of buffer allowed - 128 chars + \0
#define MAX_ARGS (3) //Maximum number of commands arguments
#define N_COMMANDS (7) //Number of command keywords deemed valid
#define SENTINEL_LIST_IDX (-1) //Index showing that there are not mtll's yet

//command keyword which are deemed valid. Initialised in commands.c
extern const char* commands[N_COMMANDS];

//Allowable types for node structs
extern enum TYPE {
    INT, 
    FLOAT, 
    CHAR, 
    STRING, 
    REFERENCE,
    NaT = -1 //NaT = Not a Type (uninitialised node)
} TYPE;

//Multi-typed linked list
extern struct mtll {
    struct node* head; //Pointer to the head node of the list
    struct mtll* next; //Pointer to the next mtll
    size_t index; //Index of this mtll
    int num_nested; //Number of times this mtll has references to other mtll
    int num_references; //Number of times this mtll has been refrenced
} mtll;

//Element of a mtll struct
extern struct node {
    void* val; //Pointer to the value which the node stores
    struct node* next; //Pointer to the next node in the node
    enum TYPE* type; //Type of value for type casting
} node;


#endif
