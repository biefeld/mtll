#include "input_parser.h"


//Checks command against defined allowed keywords
int valid_command_keyword(char* command){
    for (size_t i = 0; i < N_COMMANDS; i++){
        if (strcmp(commands[i], command) == 0){
            return 1;
        }
    }
    return 0;
}


//splits arguments and populates args, num_args
int split_arguments(char* arguments, char** args, int* num_args){

    //Clear memory which may be left over from previous calls
    memset(num_args, 0, sizeof(int));
    for (size_t i = 0; i < MAX_ARGS; i++){
        memset(args[i], 0, BUFFER);
    }

    //cursor we can move throughout the arguments string
    char* curs = arguments;

    
    //command keyword was not padded with 1 space
    //We found an extra space to start with, no good
    if (curs[0] == ' '){
        return 0;
    }

    //next space and length of cursor
    size_t* space_idx = malloc(sizeof(size_t));
    size_t* curs_len = malloc(sizeof(size_t));

    *space_idx = strcspn(curs, " ");
    *curs_len = strlen(curs);

    //There were no args, no good
    if (*curs_len == 0){
        free(space_idx);
        free(curs_len);
        return 0;
    }

    //Copy upto the next space into args[0]
    memcpy(args[0], curs, *space_idx);
    (*num_args)++;

    
    //We only found one argument, this is ok
    if (*curs_len == *space_idx){
        free(space_idx);
        free(curs_len);
        return 1;
    }

    curs += *space_idx;
    *curs_len = strlen(curs);

    //We found a single space after the 1st arg, no good
    if (curs[0] == ' ' && *curs_len == 1){
        return 0;
    }

    //+1 is to skip the space we have detected
    //we have checked if it is the only one at the end
    curs += 1;

    //We found multiple spaces after the 1st arg, no good
    if (curs[0] == ' '){
        free(space_idx);
        free(curs_len);
        return 0;
    }

    *space_idx = strcspn(curs, " ");
    *curs_len = strlen(curs);
 

    //We found a single space after the 2nd arg, no good
    if (*space_idx + 1 == *curs_len){
        free(space_idx);
        free(curs_len);
        return 0;
    }

    //copy upto the next space into args[1]
    memcpy(args[1], curs, *space_idx);
    (*num_args)++;

    //We found 2 arguments, this is ok
    if (*space_idx == *curs_len){
        free(space_idx);
        free(curs_len);
        return 1;
    }

    curs += *space_idx + 1;
    *curs_len = strlen(curs);

    //We found a 3rd argument, so just dump into args[2]
    memcpy(args[2], curs, *curs_len);
    (*num_args)++;

    free(space_idx);
    free(curs_len);
    return 1;
}


//Parses buffer and puts values into command, args and num_args
//Retunrs true if the command was valid
int parse_input(char* buffer, char* command, char** args, int* num_args){

    //find the first space
    size_t* space_idx = malloc(1 * sizeof(size_t));
    *space_idx = strcspn(buffer, " ");

    //If we have a leading space, or no spaces
    if (*space_idx == 0 || *space_idx == strlen(buffer)){
        printf("INVALID COMMAND: INPUT\n");
        free(space_idx);
        return 0;
    }

    //clear command and copy up to the first space
    memset(command, 0, BUFFER);
    memcpy(command, buffer, *space_idx);

    //Ensure that we have a valid keyword (e.g. NEW)
    if (!valid_command_keyword(command)){
        printf("INVALID COMMAND: INPUT\n");
        free(space_idx);
        return 0;
    }

    //Contains the remainder of the buffer, including the space after command
    //memcpy pads end with \0, so we allocate one more size than usual
    //len - space_idx - 2 to skip \n on the end -> no longer needed
    char* arguments = calloc(sizeof(char), (strlen(buffer) - (*space_idx) -1));
    memcpy(arguments, buffer+(*space_idx)+1, (strlen(buffer) - (*space_idx) - 2));

    //Ensure we have valid arguments for the command
    //Populates args, num_args with values
    if (!split_arguments(arguments, args, num_args)){
        printf("INVALID COMMAND: %s\n", command);
        free(arguments);
        free(space_idx);
        return 0;
    }

    //successfully parse input and populated command, arg, num_ax rgs
    free(arguments);
    free(space_idx);
    return 1;
}