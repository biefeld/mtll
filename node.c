#include "node.h"



// void node_check(){
//     float* v = malloc(sizeof(void *));
//     *v = 123.123;
//     struct node* n = node_create(v, FLOAT);
//     free(v);

//     char* v2 = malloc(sizeof(void *));
//     *v2 = 'L';
//     struct node* n2 = node_create(v2, CHAR);
//     free(v2);

//     n->next = n2;

//     //need seperate functions for printing
//     printf("%f, %d, %p\n", *(float*)n->val, n->type, n->next);
//     printf("%c, %d, %p\n", *(char*)n->next->val, n->next->type, n->next->next);
//     printf("%c, %d, %p\n", *(char*)n2->val, n2->type, n2->next);
    
//     char * t = calloc(sizeof(char), 50);
//     char * va = calloc(sizeof(char), 50);
    
//     node_type(n, t);
//     node_val(n, va);
//     printf("%s\n", t);
//     printf("%s\n", va);

//     node_type(n2, t);
//     node_val(n2, va);
//     printf("%s\n", t);
//     printf("%s\n", va);

//     free(t);
//     free(va);

//     node_free(n);
//     node_free(n2);

// }



//change this to be able to take the next element?
struct node *node_create(void* value, enum TYPE* type){
    struct node* n = malloc(sizeof(struct node));
    if (n == NULL){
        return NULL;
    }

    n->val = calloc(1, sizeof(void *));
    n->type = calloc(1, sizeof(enum TYPE*));

    if (n->val == NULL){
        free(n->val);
        free(n);
        return NULL;
    }

    //This needs to change depending on the size/type of the value
    switch (*type)
    {
    case INT:
        n->val = realloc(n->val, sizeof(int));
        memcpy(n->val, value, sizeof(int));
        break;

    case FLOAT:
        n->val = realloc(n->val, sizeof(float));
        memcpy(n->val, value, sizeof(float));
        break;

    case CHAR:
        n->val = realloc(n->val, strlen(value) + 1);
        strcpy((void*)n->val, value);
        break;

    case STRING:
        n->val = realloc(n->val, strlen(value) + 1);
        strcpy((void*)n->val, value);
        char* x = n->val + strlen(value) - 1;
        memcpy(x, "\0", 1); //Hacky way to trim trailing \n 
        break;
    
    default:
        break;
    }

    memcpy(n->type, type, sizeof(enum TYPE));
    //n->type = type; //maybe this has to be memcpy'd somehow
    n->next = NULL;

    return n;
}

void node_free(struct node* n){
    free(n->val);
    free(n->type);
    free(n);
    return;
}


void node_val(struct node * n, char * ret){
    if (*n->type == NaT){
        return;
    }

    switch (*n->type)
    {
    case INT:
        sprintf(ret, "%d", *(int*)n->val);
        break;
    case FLOAT: //needs to be 2dp
        sprintf(ret, "%.2f",*(float*)n->val);
        break;
    case CHAR:
        sprintf(ret, "%c",*(char*)n->val);
        break;
    case STRING:
        strcpy(ret, n->val);
        break;
    case REFERENCE:
        strcpy(ret, "{placeholder}");
        break;
    
    default:
        break;
    }
}

//Must free the returned char after
void node_type(struct node * n, char * ret){
    if (*n->type == NaT){
        return;
    }
    
    switch (*n->type)
    {
    case INT:
        strcpy(ret, "int");
        break;
    case FLOAT:
        strcpy(ret, "float");
        break;
    case CHAR:
        strcpy(ret, "char");
        break;
    case STRING:
        strcpy(ret, "string");
        break;
    case REFERENCE:
        strcpy(ret, "reference");
        break;

    default:
        break;
    }
}

