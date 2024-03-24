#ifndef TYPE_H
#define TYPE_H

#define BUFFER (128)
#define MAX_ARGS (3)
#define N_COMMANDS (7)

extern char* commands[N_COMMANDS];

enum TYPE {INT, FLOAT, CHAR, STRING, REFERENCE};

#endif
