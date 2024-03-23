#include "commands.h"

char* commands[N_COMMANDS] = {"NEW", "VIEW", "TYPE", 
                                   "REMOVE", "INSERT", "DELETE", "VIEW-NESTED"};


int valid_list_idx(char* list_idx, struct mtll* mtll_head){
    //We have an empty list
    if (mtll_head == NULL){
        return 0;
    }
    return 0;
}

//Could just call valid_list_idx here?
int valid_node_idx(char* list_idx, char* idx, struct mtll* mtll_head){
    if (mtll_head == NULL){
        return 0;
    }
    return 0;
}

//return 1 if valid, 0 if not. Copy the parsed value into ret.
int valid_value(char* value, void* ret, enum TYPE* type){

    //we need to check if we have more than 1?
    if (sscanf(value, "%d", (int*)ret) == 1){
        printf("Integer\n");
        *type = INT;
        return 1;
    }
    if (sscanf(value, "%f", (float*)ret) == 1){
        *type = FLOAT;
        printf("Float\n");
        return 1;
    }
    if (strlen(value) == 1 && isprint(value)){
        *type = CHAR;
        printf("Char\n");
        return 1;
    }

    //check for { or } -> {<n>} with valid n sets *type = REFERENCE; otherwise ret 0;

    *type = STRING;
    printf("String\n");
    return 1;
    //scanf("%d") -> ignoring whitespace is good so long as we have 1
    //scanf("%f") -> ignoring whitespace is good so long as we have 1
    //strlen() == 1 and isprint() == true
    //string otherwise, including just a newline and lead/trail whitespace
    //  - not including { or } except:
    //if we have {<n>}, we need to check n for being a valid, simple list
}


int parse_values(size_t* num_vals, void** values, enum TYPE* types){
    char* buff = calloc(sizeof(char), 128); //this will not do
    void* ret = calloc(sizeof(void), 128);
    enum TYPE* type = calloc(sizeof(enum TYPE), 1);
    size_t* read = calloc(sizeof(size_t), 1);
    int* valid = calloc(sizeof(int), 1);

    *read = 0;
    *valid = 1;
    // printf("WE OUT HERE %ld|%ld\n", *read, *num_vals);

    //We are using 2 buffers... no good
    while ((*read) != *(num_vals) && fgets(buff, 128, stdin) != NULL){
        (*read)++;
        if (!valid_value(buff, ret, type)){
            valid = 0;
        }
        // printf("%s\n", (char*)ret);
        //slot into values[*read] and types[*read];
    }

    free(buff);
    free(ret);
    free(type);

    if (valid && (*read) == (*num_vals)){
        free(read);
        free(valid);
        return 1;
    }

    free(read);
    free(valid);
    return 0;
    //Open a buffer, read input. We need to wait until the end to say if it was invalid
    //Use valid_value to validat each one -> can be reused for INSERT
}


int new(char* n, size_t* next_list_idx, struct mtll* head){
    //Little bit of additional argument validation
    char* curs = n;
    while (*curs){
        if (*curs < '0' || *curs > '9'){
            return 0;
        }
        curs++;
    }

    //allocate n spaces of void * so we can access n elements
    void** values = calloc(sizeof(void *), atoi(n));
    enum TYPE* types = calloc(sizeof(enum TYPE), atoi(n));

    //Convert n into size_t pointer
    size_t* num_nodes = calloc(sizeof(size_t), 1);
    *num_nodes = atoi(n);

    //If the values we got were not value, return and free
    if (!parse_values(num_nodes, values, types)){
        free(types);
        free(num_nodes);
        free(values);
        return 0;
    }

    //Make new node
    printf("Making new mtll\n");
    struct mtll* new = mtll_create(num_nodes, next_list_idx, values, types);

    free(num_nodes);

    //Slot into last spot starting from head
    //We wont have access to the 'new' pointer, as its on the stack
    //WE will have access to the value 'new' points to
    //We just need make a copy of the pointer either into head or the last element of the mtll list
    if (head == NULL){
        head = new;
    }else{
        ;//scan to end and slot into pointer ->next
        //while curr != NULL, continue. Then set curr->next = new;
    }

    (*next_list_idx)++;

    //Free what we used as temp storage -> we use memcpy to get the values out
    free(types);
    free(values);
    return 1;
}

int type(char* list_idx, struct mtll* mtll_head){
    if (!valid_list_idx(list_idx, mtll_head)){
        return 0;
    }
    printf("Displaying types\n");
    return 1;
}

int view(char* list_idx, struct mtll* mtll_head){
    if (!valid_list_idx(list_idx, mtll_head) && strcmp(list_idx, "ALL") != 0){
        return 0;
    }
    printf("Display elements\n");
    return 1;
}

int rmv(char* list_idx, struct mtll* mtll_head){
    if (!valid_list_idx(list_idx, mtll_head)){
        return 0;
    }
    printf("Removing mtll\n");
    return 1;
}

int insert(char* list_idx, char* idx, char* val, struct mtll* mtll_head){
    if (!valid_list_idx(list_idx, mtll_head)){
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
    if (!valid_list_idx(list_idx, mtll_head)){
        return 0;
    }
    if (!valid_node_idx(list_idx, idx, mtll_head)){
        return 0;
    }
    printf("Deleting element\n");
    return 1;
}

int view_nested(char* list_idx, struct mtll* mtll_head){
    if (!valid_list_idx(list_idx, mtll_head)){
        return 0;
    }
    printf("Displaying elements nested\n");
    return 1;
}


//One big distributor to the function associated with command
//Checks if we have the correct number of args for the command
//Returns the result of the command function this calls
int call_command(char* command, char** args, int* num_args, 
                                struct mtll* mtll_head,  size_t* next_list_idx){
    
    if (strcmp(command, "NEW") == 0 && *num_args == 1){
        return new(args[0], next_list_idx, mtll_head);
    }

    if (strcmp(command, "TYPE") == 0 && *num_args == 1){
        return type(args[0], mtll_head);
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