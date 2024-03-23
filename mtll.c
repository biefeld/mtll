#include "mtll.h"


// void mtll_check(){
//     size_t* next_idx = calloc(sizeof(size_t), 1);
//     size_t* num_nodes = calloc(sizeof(size_t), 1);
//     void** values = calloc(sizeof(void*), 5);
//     char* a = malloc(sizeof(char));
//     *a = 'a';
//     for (int i = 0; i < 5; i++){
//         values[i] = calloc(sizeof(char), 1);
//         memcpy(values[i], a, 1);
//         (*a)++;
//     }
//     enum TYPE* types = calloc(sizeof(enum TYPE), 5);
//     for (int i = 0; i < 5; i++){
//         types[i] = CHAR;
//     }
//     *num_nodes = 5;
//     *next_idx = 4;

//     struct mtll* m = mtll_create(num_nodes, next_idx, values, types);

//     for (int i = 0; i < 5; i++){
//         free(values[i]);
//     }

//     free(a);
//     free(values);
//     free(types);
//     free(num_nodes);
//     free(next_idx);

//     printf("Head:%p Next:%p Idx:%ld Nested?:%d References:%d\n", m->head, m->next, m->index, m->has_nested, m->num_references);
//     struct node* curr = m->head;
//     while (curr != NULL){
//         printf("%p: %c, %d, %p\n", curr, *(char*)curr->val, curr->type, curr->next);
//         curr = curr->next;

//     }
//     mtll_view(m);
//     mtll_type(m);
//     mtll_view_all(m);
//     mtll_free(m);


// }

struct mtll *mtll_create(size_t * num_nodes, size_t* next_index, void** values, enum TYPE ** types){
    struct mtll* m = malloc(sizeof(struct mtll));
    if (m == NULL){
        return NULL;
    }

    m->head = node_create(values[0], types[0]);
    struct node* curr = m->head;

    for (size_t i = 1; i < *num_nodes; i++){
        curr->next = node_create(values[i], types[i]);
        curr = curr->next;
    }

    // m->head = NULL;
    m->next = NULL;
    m->index = *next_index;
    m->has_nested = 0;
    m->num_references = 0;

    return m;
}

void mtll_free(struct mtll * m){
    //call mtll_remove until we head = NULL?
    struct node* cursor = m->head;
    struct node* next;
    while (cursor != NULL){
        next = cursor->next;
        node_free(cursor);
        cursor = next;
    }
    // node_free(curr);
    free(m);
}


void mtll_remove(struct mtll * head, size_t n){
    struct mtll* curr = head;
    while (curr != NULL){
        if (curr->index == n){
            ; //remove here
            break;
        }
        curr = curr->next;
    }
}



void mtll_view(struct mtll * m){
    struct node* curr = m->head;
    char* val = calloc(sizeof(char), 40);

    while (curr->next != NULL){
        node_val(curr, val);
        printf("%s -> ", val);
        curr = curr->next;
    }

    node_val(curr, val);
    printf("%s\n", val);
    
    free(val);
}

void mtll_type(struct mtll * m){
    struct node* curr = m->head;
    char* val = calloc(sizeof(char), 40);

    while (curr->next != NULL){
        node_type(curr, val);
        printf("%s -> ", val);
        curr = curr->next;
    }

    node_type(curr, val);
    printf("%s\n", val);
    
    free(val);
}

void mtll_view_all(struct mtll * head){
    struct mtll* curr = head;
    int* num = calloc(sizeof(int), 1);
    while (curr != NULL){
        (*num)++;
        curr = curr->next;
    }
    printf("Number of lists: %d\n", *num);
    curr = head;
    while (curr != NULL){
        printf("List %ld\n", curr->index);
        curr = curr->next;
    }
    free(num);
}



void mtll_append(struct mtll* head, struct mtll* new){
    if (head->index == -1){
        // printf("Adding new head\n");
        // head = new;
        memcpy(head, new, sizeof(struct mtll));
    }else{
        struct mtll* cursor = head;
        while (cursor->next != NULL){
            cursor = cursor->next;
        }
        cursor->next = new;
    }
}


void mtll_free_all(struct mtll* head){
    struct mtll* cursor = head;
    struct mtll* next;
    while(cursor != NULL){
        // printf("Freeing: %ld\n", cursor->index);
        next = cursor->next;
        mtll_free(cursor);
        cursor = next;
    }
    // mtll_free(cursor);
}