#include "commands.h"

char* commands[N_COMMANDS] = {"NEW", "VIEW", "TYPE", 
                                   "REMOVE", "INSERT", "DELETE", "VIEW-NESTED"};


//Could just call mtll_valid_idx here?
int valid_node_idx(char* list_idx, char* idx, struct mtll* mtll_head){
    if (mtll_head == NULL){
        return 0;
    }
    return 0;
}

//return 1 if valid, 0 if not. Copy the parsed value into ret.
int valid_value(char* value, void* ret, enum TYPE* type){

    char* garbage = calloc(128, sizeof(char)); //this will not do

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

    //check for { or } -> {<n>} with valid n sets *type = REFERENCE; otherwise ret 0;
    //if we have {<n>}, we need to check n for being a valid, simple list

    memcpy(ret, value, strlen(value));
    *type = STRING;
    // printf("String\n");
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

    //If we have received num_vals lots of valid values, all good
    if (valid && (*num_read) == (*num_vals)){
        free(num_read);
        free(valid);
        return 1;
    }

    //in any other case, not good
    free(num_read);
    free(valid);
    return 0;
}






//==============================COMMANDS======================================//


int new(char* n, size_t* next_list_idx, struct mtll* head){
    //Little bit of additional argument validation
    char* curs = n;
    while (*curs){
        if (*curs < '0' || *curs > '9'){
            return 0;
        }
        curs++;
    }

    //Convert n into size_t pointer
    size_t* num_nodes = calloc(1, sizeof(size_t));
    *num_nodes = atoi(n);

    //points to first element of list of void*, which initially point to NULL
    void** values = calloc(*num_nodes, sizeof(void *));
    
    //points to first element in list of TYPE*
    enum TYPE** types = calloc(*num_nodes, sizeof(enum TYPE *));
    
    //allocate space which each TYPE* points to
    for (size_t i = 0; i < *num_nodes; i++){
        types[i] = calloc(1, sizeof(enum TYPE));
        values[i] = calloc(1, sizeof(void));
    }


    //If the values we got were not value, return and free
    //parse_values will read, validate and populate (values, types)
    if (!parse_values(num_nodes, values, types)){
        free(num_nodes);
        for (size_t i = 0; i < atoi(n); i++){
            free(types[i]);
            free(values[i]);
        }
        free(types);
        free(values);
        return 0;
    }

    // for (size_t i = 0; i < atoi(n); i++){
    //     printf("%d\n", *types[i]);
    //     printf("%s\n", (char*)values[i]);
    // }

    //Make new mtll
    // printf("Making new mtll\n");
    struct mtll* new = mtll_create(num_nodes, next_list_idx, values, types);

    free(num_nodes);

    //insert new mtll into mtll_list
    mtll_append(head, new);
    // mtll_view_all(head);

    free(new);

    (*next_list_idx)++;

    //Free what we used as temp storage -> we use memcpy to get the values out
    for (size_t i = 0; i < atoi(n); i++){
        free(types[i]);
        free(values[i]);
    }
    free(types);
    free(values);
    return 1;
}

int view_type(char* list_idx, struct mtll* mtll_head){
    // if (!mtll_valid_idx(list_idx, mtll_head)){
    //     return 0;
    // }
    return mtll_type(list_idx, mtll_head);
    // printf("Displaying types\n");
}

int view(char* list_idx, struct mtll* mtll_head){
    if (strcmp(list_idx, "ALL") == 0){
        mtll_view_all(mtll_head);
        return 1;
    }
    
    return mtll_view(list_idx, mtll_head);

}

int rmv(char* list_idx, struct mtll* mtll_head){
    if (!mtll_valid_idx(list_idx, mtll_head)){
        return 0;
    }
    printf("Removing mtll\n");
    return 1;
}

int insert(char* list_idx, char* idx, char* val, struct mtll* mtll_head){
    if (!mtll_valid_idx(list_idx, mtll_head)){
        return 0;
    }
    if (!valid_node_idx(list_idx, idx, mtll_head)){
        return 0;
    }
    
    char* ret = calloc(128, sizeof(char)); //This will not do
    enum TYPE* type = calloc(1, sizeof(enum TYPE)); //This will not do
    if (!valid_value(val, ret, type)){
        free(ret);
        return 0;
    }
    printf("Inserting element\n");
    free(ret);
    return 1;
}

int delete(char* list_idx, char* idx, struct mtll* mtll_head){
    if (!mtll_valid_idx(list_idx, mtll_head)){
        return 0;
    }
    if (!valid_node_idx(list_idx, idx, mtll_head)){
        return 0;
    }
    printf("Deleting element\n");
    return 1;
}

int view_nested(char* list_idx, struct mtll* mtll_head){
    if (!mtll_valid_idx(list_idx, mtll_head)){
        return 0;
    }
    printf("Displaying elements nested\n");
    return 1;
}

//============================================================================//





//One big distributor to the function associated with command
//Checks if we have the correct number of args for the command
//Returns the result of the command function this calls
int call_command(char* command, char** args, int* num_args, 
                                struct mtll* mtll_head,  size_t* next_list_idx){
    
    if (strcmp(command, "NEW") == 0 && *num_args == 1){
        return new(args[0], next_list_idx, mtll_head);
    }

    if (strcmp(command, "TYPE") == 0 && *num_args == 1){
        return view_type(args[0], mtll_head);
    }

    if (strcmp(command, "VIEW") == 0 && *num_args == 1){
        return view(args[0], mtll_head);
    }

    if (strcmp(command, "REMOVE") == 0 && *num_args == 1){
        return rmv(args[0], mtll_head);
    }

    if (strcmp(command, "INSERT") == 0 && *num_args == 3){
        return insert(args[0], args[1], args[2], mtll_head);
    }

    if (strcmp(command, "DELETE") == 0 && *num_args == 2){
        return delete(args[0], args[1], mtll_head);
    }

    if (strcmp(command, "VIEW-NESTED") == 0 && *num_args == 1){
        return view_nested(args[0], mtll_head);
    }

    return 0;
}