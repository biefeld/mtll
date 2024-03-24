#include "input_parser.h"



//return 1 if valid, 0 if not. Copy the parsed value into ret.
int valid_value(char* value, void* ret, enum TYPE* type){

    char* garbage = calloc(128, sizeof(char)); //this will not do

    //Sometimes some of the last buffer makes its way in and messes things up 
    memset(ret, 0, strlen(ret));

    if (sscanf(value, "%d%s", (int*)ret, garbage) == 1){
        // printf("Integer\n");
        *type = INT;
        free(garbage);
        return 1;
    }

    if (sscanf(value, "%f%s", (float*)ret, garbage) == 1){
        *type = FLOAT;
        // printf("Float\n");
        free(garbage);
        return 1;
    }

    free(garbage);


    //-1 is to remove the newline counted
    if (strlen(value) - 1 == 1 && isprint(value[0])){
        memcpy(ret, value, 1);
        *type = CHAR;
        // printf("Char\n");
        return 1;
    }


    size_t* open_brace_idx = calloc(1, sizeof(size_t));
    size_t* close_brace_idx = calloc(1, sizeof(size_t));
    *open_brace_idx = strcspn(value, "{");
    *close_brace_idx = strcspn(value, "}");

    if (*open_brace_idx == strlen(value) && *close_brace_idx == strlen(value)){
        free(open_brace_idx);
        free(close_brace_idx);
        memcpy(ret, value, strlen(value));
        *type = STRING;
        // printf("String\n");
        return 1;
    }


    if (*open_brace_idx != 0){
        free(open_brace_idx);
        free(close_brace_idx);
        return 0;
    }

     if (*close_brace_idx != 2 && strlen(value) != 2){
        free(open_brace_idx);
        free(close_brace_idx);
        return 0;
    }



    free(open_brace_idx);
    free(close_brace_idx);

    //validate its a valid reference

    //how do i want to save the value of the reference?

    //I will also have to tackle how REFERENCE is printed

    //when we get to the mtll_create function, we should check if any types are REFERENCES

    //if so, set the is_nested flag true, and referecnes++ for the referenced list

    // memcpy(ret, value, strlen(value));
    *type = REFERENCE;

    return 1;
}


void copy_val_type(enum TYPE** types, void** values, enum TYPE* type_read, 
                                                      void* val_read, size_t n){
    memcpy(types[n], type_read, sizeof(enum TYPE));


    switch (*type_read)
    {
    case INT:
        values[n] = realloc(values[n], sizeof(int));
        memcpy(values[n], val_read, sizeof(int));
        break;

    case FLOAT:
        values[n] = realloc(values[n], sizeof(float));
        memcpy(values[n], val_read, sizeof(float));
        break;

    case CHAR:
        values[n] = realloc(values[n], strlen(val_read) + 1);
        strcpy((void*)values[n], val_read);
        // memcpy(values[n], val_read, sizeof(char));
        break;

    case STRING:
        // printf("len:%ld. new_line:%ld\n", strlen(val_read),strcspn(val_read, "\n"));
        values[n] = realloc(values[n], strlen(val_read) + 1);
        // memcpy(values[n], val_read, strlen(val_read) - 1);
        strcpy((void*)values[n], val_read);
        break;
    
    default:
        break;
    }

    return;
}


int parse_values(size_t* num_vals, void** values, enum TYPE** types){
    char* buff = calloc(128, sizeof(char)); //this will not do


    //this will be sloted into values
    void* val_read = calloc(128, sizeof(void));

    //this will be memcpy'd into types
    enum TYPE* type_read = calloc(1, sizeof(enum TYPE));

    //the number of values we have read so far
    size_t* num_read = calloc(1, sizeof(size_t));


    int* valid = calloc(1, sizeof(int));
    *valid = 1;

    //read num_vals values OR break if we recieve EOF
    while ((*num_read) != *(num_vals) && fgets(buff, 128, stdin) != NULL){
        //set flag if we get an invalid value, but keep reading
        if (!valid_value(buff, val_read, type_read)){
            *valid = 0;
        }

        copy_val_type(types, values, type_read, val_read, *num_read);

        (*num_read)++;
    }

    //free memory
    free(buff);
    free(val_read);
    free(type_read);

    //we returned early for some reason
    if ((*num_read) != *(num_vals)){
        free(num_read);
        free(valid);
        return -1;
    }

    //If we have received num_vals lots of valid values, all good
    if (*valid && (*num_read) == (*num_vals)){
        free(num_read);
        free(valid);
        return 1;
    }

    //in any other case, not good
    free(num_read);
    free(valid);
    return 0;
}

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
 

    // //We found a single space after the 2nd arg, no good -> this is fine act
    // if (*space_idx + 1 == *curs_len){
    //     free(space_idx);
    //     free(curs_len);
    //     return 0;
    // }

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