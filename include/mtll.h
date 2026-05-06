#ifndef MTLL_H
#define MTLL_H

#include "node.h"
#include "input_parser.h"
#include "structures.h"


//Allocations and freeing

// Create new mtll with nodes of given values, types. Must specify the index. 
extern struct mtll* mtll_create(size_t* num_nodes, size_t* next_index, 
                                              void** values, enum TYPE** types);

// Free all data managed by mtll*. Not safe to use if nesting lists (use mtll_remove).
extern void mtll_free(struct mtll* m);

// Free all data managed by mtll**. Not safe to use if nesting lists (use mtll_remove).
extern void mtll_free_all(struct mtll* head);


//Commands

// Print values of an mtll at index. Equivelant to VIEW command.
extern int mtll_view(char* list_idx, struct mtll** head_ptr,  int nested);

// Print all mtll with indices. Equivelant to VIEW-ALL command.
extern void mtll_view_all(struct mtll** head_ptr);

// Print values of an mtll at index, with nested lists expanded. Equivelant to VIEW-NESTED command.
extern int mtll_view_nested(char* list_idx, struct mtll** head_ptr);

// Print types of an mtll at index. Equivelant to TYPE command.
extern int mtll_type(char* list_idx, struct mtll** head_ptr);

// Remove mtll of an index, handling nesting. Equivelant to REMOVE command.
extern int mtll_remove(char* list_idx, struct mtll** head_ptr);

// Insert value at index of an mtll of a given index. Equivelant to INSERT command.
extern int mtll_insert(char* list_idx, char* idx, char* val, 
                                                        struct mtll** head_ptr);

// Remove value at index of an mtll of a given index. Equivelant to DELETE command.
extern int mtll_delete(char* list_idx, char* idx, struct mtll** head_ptr);


//Utility
extern void mtll_post_view(char* list_idx, struct mtll** head_ptr);

extern void mtll_append(struct mtll** head_ptr, struct mtll* new);

extern struct mtll* mtll_valid_idx(char* idx, struct mtll* head);

extern int mtll_valid_node_idx(char* idx);

extern void mtll_length(struct mtll* m, int* m_len);

#endif
