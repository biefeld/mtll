#include "node.h"

//Create new node
struct node *node_create(void* value, enum TYPE* type){
    struct node* n = malloc(sizeof(struct node));
    if (n == NULL){
        return NULL;
    }

    n->val = calloc(1, sizeof(void *));

    if (n->val == NULL){
        free(n->val);
        free(n);
        return NULL;
    }

    n->type = calloc(1, sizeof(enum TYPE*));

    if (n->type == NULL){
        free(n->val);
        free(n->type);
        free(n);
        return NULL;
    }


    //Realloc val based on the type, and copy over memory from value
    switch (*type)
    {
    case INT:
        n->val = realloc(n->val, sizeof(int));
        if (n->val == NULL){
            free(n->val);
            free(n->type);
            free(n);
            return NULL;
        }

        memcpy(n->val, value, sizeof(int));
        break;

    case FLOAT:
        n->val = realloc(n->val, sizeof(float));
        if (n->val == NULL){
            free(n->val);
            free(n->type);
            free(n);
            return NULL;
        }

        memcpy(n->val, value, sizeof(float));
        break;

    case CHAR:
        n->val = realloc(n->val, strlen(value) + 1);
        if (n->val == NULL){
            free(n->val);
            free(n->type);
            free(n);
            return NULL;
        }

        strcpy((void*)n->val, value);
        break;

    case STRING:
        n->val = realloc(n->val, strlen(value) + 1);
        if (n->val == NULL){
            free(n->val);
            free(n->type);
            free(n);
            return NULL;
        }

        strcpy((void*)n->val, value);
        char* x = (char*)n->val + strcspn(value, "\n");
        memcpy(x, "\0", 1); //Way to trim trailing \n 
        break;

    case REFERENCE:
        n->val = realloc(n->val, sizeof(struct mtll*));
        if (n->val == NULL){
            free(n->val);
            free(n->type);
            free(n);
            return NULL;
        }

        memcpy(n->val, value, sizeof(struct mtll*));
        break;
    
    default:
        break;
    }

    memcpy(n->type, type, sizeof(enum TYPE));
    n->next = NULL;

    return n;
}

//Free data associated with node n
void node_free(struct node* n){
    free(n->val);
    free(n->type);
    free(n);
    return;
}

//Populates ret with node n's value
void node_val(struct node* n, char* ret){
    //There is no type
    if (*n->type == NaT){
        return;
    }

    switch (*n->type)
    {
    case INT:
        sprintf(ret, "%d", *(int*)n->val);
        break;
    case FLOAT:
        sprintf(ret, "%.2f",*(float*)n->val); //needs to be 2dp
        break;
    case CHAR:
        sprintf(ret, "%c",*(char*)n->val);
        break;
    case STRING:
        strcpy(ret, (char*)n->val);
        break;
    case REFERENCE:
        //<idx> = *(struct mtll**)(n->val))->index
        printf("{List %ld", (*(struct mtll**)(n->val))->index);
        strcpy(ret, "}");
        break;
    
    default:
        break;
    }
}

//Populates ret with node n's type
void node_type(struct node* n, char* ret){
    //There is no type
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
