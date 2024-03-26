#include "mtll.h"
#include "node.h"
#include "input_parser.h"
#include "commands.h"
#include "structures.h"


int main(int argc, char** argv) {

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

    //Allocate pointer while holds the address of a pointer to the first mtll
    struct mtll** mtll_head_ptr = calloc(1, sizeof(struct mtll*));
    *mtll_head_ptr = calloc(1, sizeof(struct mtll));
    (*mtll_head_ptr)->index = -1; //sentinel value to indicate not initialised

    //Allocate pointer to the next_list_idx, and set it pointing to the value 0
    size_t* next_list_idx = calloc(1, sizeof(size_t));


    //For each line of stdin, parse input and call command if valid
    while(fgets(buffer, BUFFER, stdin) != NULL){
        //parse_input will populate command, args, num_args during parsing
        if (parse_input(buffer, command, args, num_args)){
            //call_command will call the function associated with command
            if (!call_command(command, args, num_args,
                                             mtll_head_ptr, next_list_idx)){
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
