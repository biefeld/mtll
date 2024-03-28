#include "mtll.h"

//Create new mtll, populate with values and types. Set index to next_index
//Returns the created mtll, or NULL if it could not be created.
struct mtll *mtll_create(size_t * num_nodes, size_t* next_index, void** values
                                                        , enum TYPE ** types){

    struct mtll* m = calloc(1, sizeof(struct mtll));
    if (m == NULL){
        free(m);
        return NULL;
    }

    //Set default values
    m->next = NULL;
    m->index = *next_index;
    m->num_nested = 0;
    m->num_references = 0;

    //No nodes - create empty list with first node having sentinel type NaT.
    if (*num_nodes == 0){
        int* value = calloc(1, sizeof(int));
        enum TYPE* type = malloc(sizeof(enum TYPE));
        *type = NaT;

        //Set head of mtll to the sentinel node
        m->head = node_create((void*)value, type);

        free(value);
        free(type);
        return m;
    }

    //Incerement the number of references we have as we go
    if(*types[0] == REFERENCE){
        m->num_nested++;
    }

    //Create head node
    m->head = node_create(values[0], types[0]);

    //Populate rest of list with nodes
    struct node* curr = m->head;
    for (size_t i = 1; i < *num_nodes; i++){
        if(*types[i] == REFERENCE){
            m->num_nested++;
        }

        curr->next = node_create(values[i], types[i]);
        curr = curr->next;
    }

    return m;
}

//Frees all data associated with mtll m
void mtll_free(struct mtll* m){
    struct node* cursor = m->head;
    struct node* next;

    //Free all node data
    while (cursor != NULL){
        next = cursor->next;
        node_free(cursor);
        cursor = next;
    }

    //Free the mtll struct
    free(m);
}

//Frees data for all mtll's
void mtll_free_all(struct mtll* head){
    struct mtll* cursor = head;
    struct mtll* next;

    while(cursor != NULL){
        next = cursor->next;
        mtll_free(cursor);
        cursor = next;
    }
}

//VIEW command for mtll with list_idx=index. 
//nested=1 will print out contents of the nested loops instead of <List n>
int mtll_view(char* list_idx, struct mtll** head_ptr, int nested){
    //We cannot print uninitialised mtll's
    if ((*head_ptr)->index == SENTINEL_LIST_IDX){
        return 0;
    }

    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        free(m);
        return 0;
    }

    struct node* curr = m->head;
    char* val = calloc(BUFFER, sizeof(char));

    while (curr->next != NULL){
        node_val(curr, val);
        printf("%s -> ", val);
        curr = curr->next;
    }

    node_val(curr, val);
    if(nested){
        printf("%s", val); 
    }else{
        printf("%s\n", val);
    }

    free(val);
    return 1;
}

void mtll_view_all(struct mtll** head_ptr){
    if ((*head_ptr)->index == SENTINEL_LIST_IDX){
        printf("Number of lists: 0\n");
        return;
    }


    struct mtll* curr = (*head_ptr);
    int* num = calloc(1, sizeof(int));

    while (curr != NULL){
        (*num)++;
        curr = curr->next;
    }

    printf("Number of lists: %d\n", *num);

    curr = (*head_ptr);

    while (curr != NULL){
        if (curr->num_nested != 0){
            printf("Nested %ld\n", curr->index);
        }else{
            printf("List %ld\n", curr->index);
        }
        curr = curr->next;
    }

    free(num);
}

int mtll_view_nested(char* list_idx, struct mtll** head_ptr){
    if ((*head_ptr)->index == SENTINEL_LIST_IDX){
        return 0;
    }
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }

    struct node* curr = m->head;
    char* val = calloc(BUFFER, sizeof(char));
    char* idx = calloc(BUFFER, sizeof(char));

    while (curr->next != NULL){
        if (*curr->type == REFERENCE){
            sprintf(idx, "%ld", (*(struct mtll**)(curr->val))->index);
            printf("{");
            mtll_view(idx, head_ptr, 1);
            printf("} -> ");
        }else{
            node_val(curr, val);
            printf("%s -> ", val);
        }
        curr = curr->next;
    }

    if (*curr->type == REFERENCE){
        sprintf(idx, "%ld", (*(struct mtll**)(curr->val))->index);
        printf("{");
        mtll_view(idx, head_ptr, 1);
        printf("}\n");
    }else{
        node_val(curr, val);
        printf("%s\n", val);
    }    
    free(val);
    free(idx);
    return 1;
}


int mtll_type(char* list_idx, struct mtll** head_ptr){
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }
    struct node* curr = m->head;
    char* val = calloc(BUFFER, sizeof(char));

    while (curr->next != NULL){
        node_type(curr, val);
        printf("%s -> ", val);
        curr = curr->next;
    }

    node_type(curr, val);
    printf("%s\n", val);
    
    free(val);
    return 1;
}

int mtll_remove(char* list_idx, struct mtll** head_ptr){
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }


    if(m->num_references != 0){
        return 0;
    }

    struct mtll* prev = NULL;
    struct mtll* cursor = (*head_ptr);

    //special case - we remove the head
    if ((*head_ptr)->index == m->index){
        
        //clear head and set idx = SENTINEL_LIST_IDX
        if ((*head_ptr)->next == NULL){
            mtll_free(m);
            *head_ptr = calloc(1, sizeof(struct mtll));
            (*head_ptr)->index = SENTINEL_LIST_IDX;
            printf("List %s has been removed.\n\n", list_idx);
            mtll_view_all(head_ptr);
            return 1;
        }

        //otherwise, shift head up 1 and clear old head
        (*head_ptr) = (*head_ptr)->next;
        mtll_free(cursor);

        printf("List %s has been removed.\n\n", list_idx);
        mtll_view_all(head_ptr);

        return 1;
    }

    prev = cursor;
    cursor = cursor->next;

    //evauluate if each node is a reference
    //if it is none->val->num_references--

    struct node* node_cursor = m->head;
    while(node_cursor != NULL){
        if (*node_cursor->type == REFERENCE){
            (*(struct mtll**)(node_cursor->val))->num_references--;
        }
        node_cursor = node_cursor->next;
    }

    while(cursor != NULL){
        if (cursor->index == m->index){
            prev->next = cursor->next;
            mtll_free(cursor);
            printf("List %s has been removed.\n\n", list_idx);
            mtll_view_all(head_ptr);
            return 1;
        }
        prev = cursor;
        cursor = cursor->next;
    }

    return 0;
}

int mtll_insert(char* list_idx, char* idx, char* val, struct mtll** head_ptr){

    //get the list we need to insert into using valid_list_idx
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }

    if (!mtll_valid_node_idx(idx)){
        return 0;
    }

    void* ret = calloc(BUFFER, sizeof(char));
    enum TYPE* type = calloc(1, sizeof(enum TYPE));

    //validate the value
    if (!valid_value(val, ret, type, head_ptr, list_idx)){
        free(ret);
        free(type);
        return 0;
    }


    // if (*type == REFERENCE){
    //     printf("NUMREF:%d NUMNEST:%d for idx%ld\n", m->num_references, m->num_references, m->index);
    //     if (m->num_references != 0){
    //         return 0;
    //     }
    // }

    //Get the length of the mtll
    int* m_len = calloc(1, sizeof(int));

    mtll_length(m, m_len);

    //Convert idx into a size_t for easy use
    int* s_idx = calloc(1, sizeof(int));
  
    sscanf(idx, "%d", s_idx);

    //atoi will return 0 if cannot convert -> check that any 0's are valid
    if (*s_idx == 0 && strcmp(idx, "0") != 0){
        free(s_idx);
        free(m_len);
        free(ret);
        free(type);
        return 0;
    }

    //This is a bit repeated
    if (*m_len == -1){
        switch (*type)
        {
        case INT:
            m->head->val = realloc(m->head->val, sizeof(int));
            memcpy(m->head->val, ret, sizeof(int));
            break;

        case FLOAT:
            m->head->val = realloc(m->head->val, sizeof(float));
            memcpy(m->head->val, ret, sizeof(float));
            break;

        case CHAR:
            m->head->val = realloc(m->head->val, strlen(ret) + 1);
            strcpy((void*)m->head->val, ret);
            break;

        case STRING:
            m->head->val = realloc(m->head->val, strlen(ret) + 1);
            strcpy((void*)m->head->val, ret);
            char* x = (char*)m->head->val + strcspn(m->head->val, "\n");
            memcpy(x, "\0", 1); //Hacky way to trim trailing \n 
            break;
        case REFERENCE:
            // printf("values:%p\n", *(struct mtll**)value);
            m->head->val = realloc(m->head->val, sizeof(struct mtll*));
            memcpy(m->head->val, ret, sizeof(struct mtll*));
            break;
        default:
            break;
        }
        memcpy(m->head->type, type, sizeof(enum TYPE));
        mtll_post_view(list_idx, head_ptr);
        free(s_idx);
        free(m_len);
        free(ret);
        free(type);
        return 1;
    }

    //must be in range [-m_len-2:m_len+1]
    if (!(
        (*s_idx) >= ((-1 * (*m_len)) - 1) && 
        (*s_idx) <= (*m_len)
    )){
        free(s_idx);
        free(m_len);
        free(ret);
        free(type);
        return 0;
    }

    //if idx is negative -> insert into s_idx + m_len + 1 slot
    if (*s_idx < 0){
        *s_idx += *m_len + 1;
    }


    //actually insert into list now
    struct node* new = node_create(ret, type);
    int* pos = calloc(1, sizeof(int));
    struct node* cursor;

    //were inserting into head
    if (*s_idx == 0){
        new->next = m->head;
        m->head = new;
    } else{
        cursor = m->head;
        while (*pos != *s_idx - 1){
            cursor = cursor->next;
            (*pos)++;
        }
        struct node* next = cursor->next;
        new->next = next;
        cursor->next = new;
    }

    if (*type == REFERENCE){
        // struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
        // if (m->num_references != 0){
        //     return 0;
        // }
        m->num_nested++;
        char* idx = calloc(strcspn(val, "}"), sizeof(char));
        memcpy(idx, val + 1, strcspn(val, "}") - 1);
        struct mtll* ref = mtll_valid_idx(idx, *head_ptr);
        free(idx);
        (ref->num_references)++;
    }
    
    mtll_post_view(list_idx, head_ptr);

    free(pos);
    free(s_idx);
    free(m_len);
    free(ret);
    free(type);
    return 1;
}

int mtll_delete(char* list_idx, char* idx, struct mtll** head_ptr){

    //get the list we need to delete from using valid_list_idx
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }

    if (!mtll_valid_node_idx(idx)){
        return 0;
    }

    void* ret = calloc(BUFFER, sizeof(char));
    enum TYPE* type = calloc(1, sizeof(enum TYPE));

    //Get the length of the mtll
    int* m_len = calloc(1, sizeof(int));

    mtll_length(m, m_len);

    //we are deleting the only node in the list
    if (*m_len == 1){
        if (*m->head->type == REFERENCE){
            m->num_nested--;
            char* ref_idx = calloc(BUFFER, sizeof(char));
            sprintf(ref_idx, "%ld", (*(struct mtll**)(m->head->val))->index);
            struct mtll* ref = mtll_valid_idx(ref_idx, *head_ptr);
            (ref->num_references)--;
            free(ref_idx);
        }
        int* value = malloc(sizeof(int));
        *value = -1;
        enum TYPE* type = malloc(sizeof(enum TYPE));
        *type = NaT;
        m->head->val = realloc(m->head->val, sizeof(int));
        memcpy(m->head->val, value, sizeof(int));
        memcpy(m->head->type, type, sizeof(enum TYPE));
        
        mtll_post_view(list_idx, head_ptr);

        free(value);
        free(type);
        free(m_len);
        free(ret);
        return 1;
    }


    //Convert idx into a size_t for easy use
    int* s_idx = calloc(1, sizeof(int));
    *s_idx = atoi(idx);

    //atoi will return 0 if cannot convert -> check that any 0's are valid
    if (*s_idx == 0 && strcmp(idx, "0") != 0){
        free(s_idx);
        free(m_len);
        free(ret);
        free(type);
        return 0;
    }

    //must be in range [-m_len-1:m_len]
    if (!(
        (*s_idx) >= (-1 * (*m_len)) && 
        (*s_idx) <= ((*m_len) - 1)
    )){
        free(s_idx);
        free(m_len);
        free(ret);
        free(type);
        return 0;
    }

    //if idx is negative -> delete from s_idx + m_len slot
    if (*s_idx < 0){
        *s_idx += *m_len;
    }


    //actually delete from list now
    int* pos = calloc(1, sizeof(int));
    struct node* cursor;

    //were deleting the head
    if (*s_idx == 0){
        if (*m->head->type == REFERENCE){
            m->num_nested--;
            
            char* ref_idx = calloc(BUFFER, sizeof(char));
            sprintf(ref_idx, "%ld", (*(struct mtll**)(m->head->val))->index);
            struct mtll* ref = mtll_valid_idx(ref_idx, *head_ptr);
            (ref->num_references)--;
            free(ref_idx);
        }
        cursor = m->head;
        m->head = m->head->next;
        node_free(cursor);
    } else{
        cursor = m->head;
        while (*pos != *s_idx - 1){
            cursor = cursor->next;
            (*pos)++;
        }
        if (*cursor->next->type == REFERENCE){
            m->num_nested--;
            char* ref_idx = calloc(BUFFER, sizeof(char));
            sprintf(ref_idx, "%ld", (*(struct mtll**)(cursor->next->val))->index);
            struct mtll* ref = mtll_valid_idx(ref_idx, *head_ptr);
            (ref->num_references)--;
            free(ref_idx);
        }
        struct node* next = cursor->next;
        cursor->next = next->next;
        node_free(next);
    }

    mtll_post_view(list_idx, head_ptr);

    free(pos);
    free(s_idx);
    free(m_len);
    free(ret);
    free(type);
    return 1;
}

//View the contents of mtll with list_idx. Used after COMMANDS are called
void mtll_post_view(char* list_idx, struct mtll** head_ptr){
    //Find the mtll we are working with
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    
    //Nested or simple loop variants
    if (m->num_nested != 0){
        printf("Nested %ld: ", m->index);
    }else{
        printf("List %ld: ", m->index);
    }

    //View list with non-nested format
    mtll_view(list_idx, head_ptr, 0);
}

//Adds new to end of head_ptr (list of mtll's)
void mtll_append(struct mtll** head_ptr, struct mtll* new){

    //We are adding as the first element of the lsit
    if ((*head_ptr)->index == SENTINEL_LIST_IDX){
        memcpy((*head_ptr), new, sizeof(struct mtll));
        return;
    }

    //Scan thru to find end mtll
    struct mtll* cursor = (*head_ptr);
    while (cursor->next != NULL){
        cursor = cursor->next;
    }

    //Assign to the next element of the last mtll
    struct mtll* next_new = malloc(sizeof(struct mtll));
    cursor->next = next_new;
    memcpy(next_new, new, sizeof(struct mtll));
}

//Returns the mtll associated with idx if found, NULL if not
struct mtll* mtll_valid_idx(char* idx, struct mtll* head){
    //Uninitialised mtll
    if (head->index == SENTINEL_LIST_IDX){
        return NULL;
    }

    //If we have an invalid idx    
    for (size_t i = 0; i < strlen(idx); i++){
        if (!isdigit(idx[i])){
            return NULL;
        }
    }

    //Scan through mtll's, finding the correct idx
    struct mtll* cursor = head;
    while(cursor != NULL){
        if (cursor->index == atoi(idx)){
            return cursor;
        }
        cursor = cursor->next;
    }

    //Not found
    return NULL;
}

//Return 1 if valid, 0 if not. Must be numerical plus one sign character (opt)
int mtll_valid_node_idx(char* idx){
    //Check 1st char is digit or sing
    if (!isdigit(idx[0]) && idx[0] != '-' && idx[0] != '+'){
        return 0;
    }

    //Check rest are sign
    for (size_t i = 1; i < strlen(idx); i++){
        if (!isdigit(idx[i])){
            return 0;
        }
    }

    return 1;
}

//Puts the length of the mtll in m_len. If we have an uninitialized mtll, = -1
void mtll_length(struct mtll* m, int* m_len){
    if (*(m->head->type) == NaT){
        *m_len = -1;
        return;
    }
    
    struct node* cursor = m->head;
    while (cursor != NULL){
        (*m_len)++;
        cursor = cursor->next;
    }
}
