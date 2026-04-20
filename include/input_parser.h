#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "structures.h"


extern int valid_value(char* value, void* ret, enum TYPE* type, 
                                 struct mtll** mtll_head_ptr, char* insert_idx);

extern int parse_values(size_t* num_vals, void** values, enum TYPE** types, 
                                                   struct mtll** mtll_head_ptr);

extern int valid_command_keyword(char* command);

extern int split_arguments(char* arguments, char** args, int* num_args);

extern int parse_input(char* buffer, char* command, char** args, int* args_len);

extern void copy_val_type(enum TYPE** types, void** values, 
                                enum TYPE* type_read, void* val_read, size_t n);


#endif
