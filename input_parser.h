#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER (128)
#define MAX_ARGS (3)

#include "commands.h"

extern int parse_input(char* buffer, char* command, char** args, int* args_len);


#endif
