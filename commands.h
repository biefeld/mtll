#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "mtll.h"

#define N_COMMANDS (7)

extern char* commands[N_COMMANDS];

extern int call_command(char* command, char** args, int* args_len, struct mtll* mtll_head, size_t* next_list_idx);

#endif
