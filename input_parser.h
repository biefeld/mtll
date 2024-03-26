#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "structures.h"

extern int parse_input(char* buffer, char* command, char** args, int* args_len);

extern int valid_value(char* value, void* ret, enum TYPE* type, 
                                 struct mtll** mtll_head_ptr, char* insert_idx);

extern void copy_val_type(enum TYPE** types, void** values, 
                                enum TYPE* type_read, void* val_read, size_t n);

extern int parse_values(size_t* num_vals, void** values, enum TYPE** types, 
                                                   struct mtll** mtll_head_ptr);

#endif
