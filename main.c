#include "mtll.h"
#include "node.h"
#include "input_parser.h"
#include "commands.h"

void debug_output(char* buffer, char* command, char** args, int* num_args, size_t* next_list_idx){
    printf("\n============================\nWAS ABLE TO PARSE THE ARGUMENTS\n");
    printf("Command|%s|\n", command);
    printf("Next_list_idx=%ld\n", *next_list_idx);
    printf("Args_len|%d|\n", *num_args);
    for (int i=0; i<MAX_ARGS; i++) {
        printf("Args[%d]|%s|\n", i, args[i]);
    }
    printf("===========================\n\n");
}


int main(int argc, char** argv) {

    // node_check();
    // mtll_check();

    //Allocate memory we will use for each input
    char* buffer = calloc(BUFFER, sizeof(char));
    char* command = calloc(BUFFER, sizeof(char));
    int* num_args = calloc(1, sizeof(int));
    
    //pointer to the first element of a list of MAX_ARGS char* pointers
    //each contain a pointer to the first element of a char list
    char** args = calloc(MAX_ARGS, sizeof(char *)); 
    for (int i=0; i<MAX_ARGS; i++) {
        args[i] = calloc(BUFFER, sizeof(char));
    }

    //Allocate pointer to mtll_head, and set it pointing to NULL (no head yet)
    struct mtll** mtll_head_ptr = calloc(1, sizeof(struct mtll*));
    *mtll_head_ptr = calloc(1, sizeof(struct mtll));
    (*mtll_head_ptr)->index = -1; //sentinel value

    //Allocate pointer to the next_list_idx, and set it pointing to the value 0
    size_t* next_list_idx = calloc(1, sizeof(size_t));
    

    //For each line of stdin, parse input and call command if valid
    //parse_input will populate command, args, num_args during parsing
    //call_command will call the function associated with the command parsed

    while(fgets(buffer, BUFFER, stdin) != NULL){
        if (parse_input(buffer, command, args, num_args)){
            // debug_output(buffer, command, args, num_args, next_list_idx);

            if (!call_command(command, args, num_args, mtll_head_ptr, next_list_idx)){
                printf("INVALID COMMAND: %s\n", command);
            }
        }
    }


    //Free remaining mtll's, node's and value's
    mtll_free_all(*mtll_head_ptr);
    free(mtll_head_ptr);
   
    //Free buffer-like memory
    for (int i=0; i<MAX_ARGS; i++) {
        free(args[i]);
    }
    free(next_list_idx);
    free(args);
    free(num_args);
    free(command);
    free(buffer);

    return 0;
}
