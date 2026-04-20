#include "commands.h"

const char* commands[N_COMMANDS] = {"NEW", "VIEW", "TYPE", 
                                   "REMOVE", "INSERT", "DELETE", "VIEW-NESTED"};


//==============================COMMANDS======================================//

int new(char* n, size_t* next_list_idx, struct mtll** mtll_head_ptr){
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
        values[i] = calloc(1, sizeof(char));
    }


    //If the values we got were not value, return and free
    //parse_values will read, validate and populate (values, types)
    int* ret = calloc(1, sizeof(int));
    *ret = parse_values(num_nodes, values, types, mtll_head_ptr);

    //-1 indicates EOF early
    if (*ret == -1){
        free(ret);
        free(num_nodes);
        for (size_t i = 0; i < atoi(n); i++){
            free(types[i]);
            free(values[i]);
        }
        free(types);
        free(values);
        return 1;
    }

    //0 indicates valid values were not parsed
    if (*ret == 0){
        free(ret);
        free(num_nodes);
        for (size_t i = 0; i < atoi(n); i++){
            free(types[i]);
            free(values[i]);
        }
        free(types);
        free(values);
        return 0;
    }

    free(ret);

    //Make new mtll
    struct mtll* new = mtll_create(num_nodes, next_list_idx, values, types);

    free(num_nodes);

    //insert new mtll into mtll_list
    mtll_append(mtll_head_ptr, new);

    char* list_idx = calloc(BUFFER, sizeof(char));

    //convert size_t into char*
    snprintf(list_idx, BUFFER, "%zu", *next_list_idx);
    mtll_post_view(list_idx, mtll_head_ptr);

    free(new);
    free(list_idx);

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

int view_type(char* list_idx, struct mtll** mtll_head_ptr){
    return mtll_type(list_idx, mtll_head_ptr);
}

int view(char* list_idx, struct mtll** mtll_head_ptr){
    if (strcmp(list_idx, "ALL") == 0){
        mtll_view_all(mtll_head_ptr);
        return 1;
    }
    
    return mtll_view(list_idx, mtll_head_ptr, 0);
}

int rmv(char* list_idx, struct mtll** mtll_head_ptr){
    return mtll_remove(list_idx, mtll_head_ptr);
}

int insert(char* list_idx, char* idx, char* val, struct mtll** mtll_head_ptr){
    return mtll_insert(list_idx, idx, val, mtll_head_ptr);
}

int delete(char* list_idx, char* idx, struct mtll** mtll_head_ptr){
    return mtll_delete(list_idx, idx, mtll_head_ptr);
}

int view_nested(char* list_idx, struct mtll** mtll_head_ptr){
    return mtll_view_nested(list_idx, mtll_head_ptr);
}

//============================================================================//


//One big distributor to the function associated with command
//Checks if we have the correct number of args for the command
//Returns the result of the command function this calls
int call_command(char* command, char** args, int* num_args, 
                           struct mtll** mtll_head_ptr,  size_t* next_list_idx){
    
    if (strcmp(command, "NEW") == 0 && *num_args == 1){
        return new(args[0], next_list_idx, mtll_head_ptr);
    }

    if (strcmp(command, "TYPE") == 0 && *num_args == 1){
        return view_type(args[0], mtll_head_ptr);
    }

    if (strcmp(command, "VIEW") == 0 && *num_args == 1){
        return view(args[0], mtll_head_ptr);
    }

    if (strcmp(command, "REMOVE") == 0 && *num_args == 1){
        return rmv(args[0], mtll_head_ptr);
    }

    if (strcmp(command, "INSERT") == 0 && *num_args == 3){
        return insert(args[0], args[1], args[2], mtll_head_ptr);
    }

    if (strcmp(command, "DELETE") == 0 && *num_args == 2){
        return delete(args[0], args[1], mtll_head_ptr);
    }

    if (strcmp(command, "VIEW-NESTED") == 0 && *num_args == 1){
        return view_nested(args[0], mtll_head_ptr);
    }

    return 0;
}
