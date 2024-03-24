#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "mtll.h"
#include "type.h"
#include "input_parser.h"

extern int call_command(char* command, char** args, int* num_args, struct mtll* mtll_head,  size_t* next_list_idx);

extern int new(char* n, size_t* next_list_idx, struct mtll* head);

extern int view_type(char* list_idx, struct mtll* mtll_head);

extern int view(char* list_idx, struct mtll* mtll_head);

extern int rmv(char* list_idx, struct mtll* mtll_head);

extern int insert(char* list_idx, char* idx, char* val, struct mtll* mtll_head);

extern int delete(char* list_idx, char* idx, struct mtll* mtll_head);

extern int view_nested(char* list_idx, struct mtll* mtll_head);

#endif
