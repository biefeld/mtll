#include "input_parser.h"


//return 1 if value is valid, 0 if not. 
//Copy the parsed value into ret, type of value into type
int valid_value(char* value, void* ret, enum TYPE* type, 
                                struct mtll** mtll_head_ptr, char* insert_idx){

    //used to check if there are additional characters after scanf
    char* garbage = calloc(BUFFER, sizeof(char)); 

    //Clear the last parsed value 
    memset(ret, 0, strlen(ret));

    //If we scan just one integer
    if (sscanf(value, "%d%s", (int*)ret, garbage) == 1){
        *type = INT;
        free(garbage);
        return 1;
    }

    //If we scan just one float
    if (sscanf(value, "%f%s", (float*)ret, garbage) == 1){
        *type = FLOAT;
        free(garbage);
        return 1;
    }

    free(garbage);

    //This is to account for INSERT stripping the \n away
    int* insert_factor = calloc(1, sizeof(int));
    if (insert_idx == NULL){
        *insert_factor = 1;
    }

    //-1 is to remove the newline counted -> - 0 if we are using INSERt
    if (strlen(value) - *insert_factor == 1 && isprint(value[0]) != 0){

        //Cannot have a single brace character at all
        if (value[0] == '{' || value[0] == '}'){
            return 0;
        }

        memcpy(ret, value, 1);
        *type = CHAR;
        free(insert_factor);
        return 1;
    }


    //Determine if we have a string, reference or invalid string
    size_t* open_brace_idx = calloc(1, sizeof(size_t));
    size_t* close_brace_idx = calloc(1, sizeof(size_t));
    *open_brace_idx = strcspn(value, "{");
    *close_brace_idx = strcspn(value, "}");


    //There are no { or } characters in the string -> valid
    if (*open_brace_idx == strlen(value) && *close_brace_idx == strlen(value)){
        free(open_brace_idx);
        free(close_brace_idx);
        memcpy(ret, value, strlen(value));
        *type = STRING;
        free(insert_factor);
        return 1;
    }

    //way of fixing INSERT not copying over \n
    if (strcspn(value, "\n") == strlen(value)){
        memcpy(value+strlen(value), "\n", 1);
    }

    //we do not begin and end with { and } -> invalid reference
    if (*open_brace_idx != 0){
        free(open_brace_idx);
        free(close_brace_idx);
        free(insert_factor);
        return 0;
    }

     if (*close_brace_idx != strlen(value) - 1 - *insert_factor){
        free(open_brace_idx);
        free(close_brace_idx);
        free(insert_factor);
        return 0;
    }

    //We have non-digit characters within {} -> invalid reference
    for (size_t i = 1; i < strlen(value) - 1 - *insert_factor; i++){
        if(!isdigit(value[i])){
            free(open_brace_idx);
            free(close_brace_idx);
            free(insert_factor);
            return 0;
        }
    }

    free(open_brace_idx);
    free(close_brace_idx);

    //Extract digits we reference
    char* idx = calloc(strcspn(value, "}"), sizeof(char));
    memcpy(idx, value + 1, strcspn(value, "}") - *insert_factor);

    free(insert_factor);

    //If we are calling INSERT, and attempt to self-reference
    if(insert_idx != NULL){
        if (strcmp(idx, insert_idx) == 0){
            free(idx);
            return 0;
        }
    }


    //we have not initialised any lists yet - cant insert
    if ((*mtll_head_ptr)->index == SENTINEL_LIST_IDX){
        free(idx);
        return 0;
    }

    //List we are referencing
    struct mtll* ref_mtll = NULL;

    //Attemp to find which list we want to insert into
    struct mtll* cursor = (*mtll_head_ptr);
    while(cursor != NULL){
        if (cursor->index == atoi(idx)){
            ref_mtll = cursor;
            break;
        }
        cursor = cursor->next;
    }

    //If we could not find a valid reference
    if (ref_mtll == NULL){
        free(idx);
        return 0;
    }

    //If the nesting would create a nest too deep (ref_mtll is already nested)
    if (ref_mtll->num_nested){
        free(idx);
        return 0;
    }

    //add 1 to how many times the mtll has been referenced
    ref_mtll->num_references++;

    free(idx);

    //set the return value to the address of the list we are referencing
    //this will be processed later
    *(struct mtll**)ret = ref_mtll;
    *type = REFERENCE;

    return 1;
}

//copies val_read, type_read into values, types at index n
void copy_val_type(enum TYPE** types, void** values, enum TYPE* type_read, 
                                                      void* val_read, size_t n){
    //No realloc needed -> will always be sizeof(enum TYPE)
    memcpy(types[n], type_read, sizeof(enum TYPE));

    //realloc size of values[n] depending on what we store
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
        //same as STRING

    case STRING:
        values[n] = realloc(values[n], strlen(val_read) + 1);
        strcpy((void*)values[n], val_read);
        break;

    case REFERENCE:
        values[n] = realloc(values[n], sizeof(struct mtll*));
        *(struct mtll**)values[n] = *(struct mtll**)val_read;
        break;
    
    default:
        break;
    }

    return;
}

//Return 0 for invalid values, 1 for valid values and -1 for early EOF
//populates values, types with num_vals values
int parse_values(size_t* num_vals, void** values, enum TYPE** types
                                                , struct mtll** mtll_head_ptr){
    
    char* buff = calloc(BUFFER, sizeof(char)); 

    //this will be slotted into values
    void* val_read = calloc(BUFFER, sizeof(char));

    //this will be slotted into types
    enum TYPE* type_read = calloc(1, sizeof(enum TYPE));

    //the number of values we have read so far
    size_t* num_read = calloc(1, sizeof(size_t));

    //Flag which is set to off if we get any invalid values
    int* valid = calloc(1, sizeof(int));
    *valid = 1;

    //read num_vals values OR break if we recieve EOF
    while ((*num_read) != *(num_vals) && fgets(buff, BUFFER, stdin) != NULL){
        //set flag if we get an invalid value, but keep reading
        if (!valid_value(buff, val_read, type_read, mtll_head_ptr, NULL)){
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
//returns 0 if there are any invalid padding or similar
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
        free(space_idx);
        free(curs_len);
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
//Retunrs 1 if the command/arguments was valid
int parse_input(char* buffer, char* command, char** args, int* num_args){

    //find the first space
    size_t* space_idx = malloc(1 * sizeof(size_t));
    *space_idx = strcspn(buffer, " ");

    //If we have a leading space
    if (*space_idx == 0 ){
        printf("INVALID COMMAND: INPUT\n");
        free(space_idx);
        return 0;
    }

    //clear command and copy up to the first space
    memset(command, 0, BUFFER);

    //If we have no space
    if (*space_idx == strlen(buffer)){

        memcpy(command, buffer, *space_idx - 1);

        //If there isnt a command keyword match - print INPUT
        if (!valid_command_keyword(command)){
            printf("INVALID COMMAND: INPUT\n");
            free(space_idx);
            return 0;
        }

        //Check if there is a command that matches - print <COMMAND>
        printf("INVALID COMMAND: %s\n", command);
        free(space_idx);
        return 0;
    }

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
    char* arguments = calloc(sizeof(char), (strlen(buffer) - (*space_idx)-1));
    memcpy(arguments, buffer+(*space_idx)+1, (strlen(buffer) - (*space_idx)-2));

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
