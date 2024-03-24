#ifndef TYPE_H
#define TYPE_H

#define BUFFER (130)
#define MAX_ARGS (3)
#define N_COMMANDS (7)

extern char* commands[N_COMMANDS];

enum TYPE {INT, FLOAT, CHAR, STRING, REFERENCE, NaT=-1};

#endif
