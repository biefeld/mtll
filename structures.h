#ifndef TYPE_H
#define TYPE_H

#define BUFFER (129) //Size of buffer allowed - 128 chars + \0
#define MAX_ARGS (3) //Maximum number of commands arguments
#define N_COMMANDS (7) //Number of command keywords deemed valid

//command keyword which are deemed valid. Initialised in commands.c
extern char* commands[N_COMMANDS];

//Allowable types for node structs
enum TYPE {
    INT, 
    FLOAT, 
    CHAR, 
    STRING, 
    REFERENCE,
    NaT = -1 //NaT = Not a Type (uninitialised node)
};

//Multi-typed linked list
struct mtll {
    struct node* head; //Pointer to the head node of the list
    struct mtll* next; //Pointer to the next mtll
    size_t index; //Index of this mtll
    int num_nested; //Number of times this mtll has references to other mtll
    int num_references; //Number of times this mtll has been refrenced
};

//Element of a mtll struct
struct node {
    void* val; //Pointer to the value which the node stores
    struct node* next; //Pointer to the next node in the node
    enum TYPE* type; //Type of value for type casting
};


#endif
