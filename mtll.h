#ifndef MTLL_H
#define MTLL_H

#include "node.h"
#include "input_parser.h"
#include "type.h"

struct mtll {
    struct node* head; //Pointer to the head node of the list
    struct mtll* next; //Pointer to the next mtll
    size_t index; //Index of this mtll
    // int size; //Number of elements in the list
    int is_nested; //If this mtll has references to other mtll
    int num_references; //Number of times this mtll has been refrenced
};

//Allocations and freeing
extern struct mtll *mtll_create(size_t* num_nodes, size_t* next_index, void** values, enum TYPE** types);

extern void mtll_free(struct mtll* m);

extern void mtll_free_all(struct mtll* head);


//Commands
extern int mtll_view(char* list_idx, struct mtll** head_ptr);

extern void mtll_view_all(struct mtll** head_ptr);

extern int mtll_type(char* list_idx, struct mtll** head_ptr);

extern int mtll_remove(char* list_idx, struct mtll** head_ptr);

extern int mtll_insert(char* list_idx, char* idx, char* val, struct mtll** head_ptr);

extern int mtll_delete(char* list_idx, char* idx, struct mtll** head_ptr);


//Utility
extern void mtll_post_view(char* list_idx, struct mtll** head_ptr);

extern void mtll_append(struct mtll** head_ptr, struct mtll* new);

extern struct mtll* mtll_valid_idx(char* idx, struct mtll* head);

extern int mtll_valid_node_idx(char* idx);

extern void mtll_length(struct mtll* m, int* m_len);

#endif
