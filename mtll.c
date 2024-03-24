#include "mtll.h"

struct mtll *mtll_create(size_t * num_nodes, size_t* next_index, void** values, enum TYPE ** types){
    struct mtll* m = malloc(sizeof(struct mtll));
    if (m == NULL){
        return NULL;
    }

    m->next = NULL;
    m->index = *next_index;
    m->is_nested = 0;
    m->num_references = 0;

    //create an empty list - head node has sentinel value as value
    if (*num_nodes == 0){
        int* value = malloc(sizeof(int));
        *value = -1;
        enum TYPE* type = malloc(sizeof(enum TYPE));
        *type = NaT;
        m->head = node_create((void*)value, type);
        free(value);
        free(type);
        return m;
    }

    m->head = node_create(values[0], types[0]);
    struct node* curr = m->head;

    for (size_t i = 1; i < *num_nodes; i++){
        curr->next = node_create(values[i], types[i]);
        curr = curr->next;
    }
    return m;
}

void mtll_free(struct mtll* m){
    struct node* cursor = m->head;
    struct node* next;
    while (cursor != NULL){
        next = cursor->next;
        node_free(cursor);
        cursor = next;
    }
    free(m);
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

int mtll_view(char* list_idx, struct mtll** head_ptr){
    if ((*head_ptr)->index == -1){
        return 0;
    }
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }

    struct node* curr = m->head;
    char* val = calloc(40, sizeof(char));

    while (curr->next != NULL){
        node_val(curr, val);
        printf("%s -> ", val);
        curr = curr->next;
    }

    node_val(curr, val);
    printf("%s\n", val);
    
    free(val);
    return 1;
}

void mtll_view_all(struct mtll** head_ptr){
    if ((*head_ptr)->index == -1){
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
        if (curr->is_nested){
            printf("Nested %ld\n", curr->index);
        }else{
            printf("List %ld\n", curr->index);
        }
        curr = curr->next;
    }

    free(num);
}

int mtll_type(char* list_idx, struct mtll** head_ptr){
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m == NULL){
        return 0;
    }
    struct node* curr = m->head;
    char* val = calloc(40, sizeof(char));

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

    // printf("REMOVE %ld INDEX\n", m->index);

    struct mtll* prev = NULL;
    struct mtll* cursor = (*head_ptr);

    //special case - we remove the head
    if ((*head_ptr)->index == m->index){
        
        //clear head and set idx = -1
        if ((*head_ptr)->next == NULL){
            mtll_free(m);
            *head_ptr = calloc(1, sizeof(struct mtll));
            (*head_ptr)->index = -1;
            // //Clear nodes in the linked list
            // struct node* cursor = (*head_ptr)->head;
            // struct node* next;
            // while (cursor != NULL){
            //     next = cursor->next;
            //     node_free(cursor);
            //     cursor = next;
            // }
            
            // //set head to sentinel value
            // (*head_ptr)->index = -1;
            printf("List %s has been removed.\n\n", list_idx);
            mtll_view_all(head_ptr);
            return 1;
        }

        //otherwise, shift head up 1 and clear old head
        (*head_ptr) = (*head_ptr)->next;
        mtll_free(cursor);

        printf("List %s has been removed.\n\n", list_idx);
        mtll_view_all(head_ptr);

        // printf("THIS REMOVE IS NOT WORKING\n");
        return 1;
    }

    prev = cursor;
    cursor = cursor->next;

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

    void* ret = calloc(128, sizeof(void));
    enum TYPE* type = calloc(1, sizeof(enum TYPE));

    //validate the value
    if (!valid_value(val, ret, type)){
        free(ret);
        free(type);
        return 0;
    }

    //Get the length of the mtll
    int* m_len = calloc(1, sizeof(int));

    mtll_length(m, m_len);

    //Convert idx into a size_t for easy use
    int* s_idx = calloc(1, sizeof(int));
  
    // if (!isdigit(idx[0]) && idx[0] != '-' && idx[0] != '+'){
    //     free(s_idx);
    //     free(m_len);
    //     free(ret);
    //     free(type);
    //     return 0;
    // }
    // for (size_t i = 1; i < strlen(idx); i++){
    //     if (!isdigit(idx[i])){
    //         free(s_idx);
    //         free(m_len);
    //         free(ret);
    //         free(type);
    //         return 0;
    //     }
    // }
    sscanf(idx, "%d", s_idx);
    // printf("idx:%s, s_idx:%d\n", idx, *s_idx);
    // *s_idx = atoi(idx);

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
            // printf("VALUE TO INSERT:|%s|\n", (char*)ret);
            //may be of use: strcspn(val_read, "\n")
            m->head->val = realloc(m->head->val, strlen(ret) + 1);
            strcpy((void*)m->head->val, ret);
            char* x = m->head->val + strcspn(m->head->val, "\n");
            memcpy(x, "\0", 1); //Hacky way to trim trailing \n 
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

    // printf("%d|%d -> %d\n", *s_idx, *m_len, ;

    // printf("[%d:%d]\n", ((*m_len)*-1)-1, (*m_len));

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
    // printf("Inserting %s into index:%d\n", val, *s_idx);

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

    void* ret = calloc(128, sizeof(void));
    enum TYPE* type = calloc(1, sizeof(enum TYPE));

    //Get the length of the mtll
    int* m_len = calloc(1, sizeof(int));

    mtll_length(m, m_len);

    

    if (*m_len == 1){
        int* value = malloc(sizeof(int));
        *value = -1;
        enum TYPE* type = malloc(sizeof(enum TYPE));
        *type = NaT;
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
    // printf("Inserting %s into index:%d\n", val, *s_idx);

    int* pos = calloc(1, sizeof(int));
    struct node* cursor;

    //were deleting the head
    if (*s_idx == 0){
        cursor = m->head;
        m->head = m->head->next;
        node_free(cursor);
    } else{
        cursor = m->head;
        while (*pos != *s_idx - 1){
            cursor = cursor->next;
            (*pos)++;
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

void mtll_post_view(char* list_idx, struct mtll** head_ptr){
    struct mtll* m = mtll_valid_idx(list_idx, *head_ptr);
    if (m->is_nested){
        printf("Nested %ld: ", m->index);
    }else{
        printf("List %ld: ", m->index);
    }
    mtll_view(list_idx, head_ptr);
}

void mtll_append(struct mtll** head_ptr, struct mtll* new){
    //printf("HEADINDEX%ld\n", head->index);
    if ((*head_ptr)->index == -1){
        // printf("Adding new head\n");
        // head = new;
        memcpy((*head_ptr), new, sizeof(struct mtll));
    }else{
        struct mtll* cursor = (*head_ptr);
        while (cursor->next != NULL){
            cursor = cursor->next;
        }
        struct mtll* next_new = malloc(sizeof(struct mtll));
        cursor->next = next_new;
        memcpy(next_new, new, sizeof(struct mtll));
        // cursor->next = new;
    }
}

struct mtll* mtll_valid_idx(char* idx, struct mtll* head){
    if (head->index == -1){
        return NULL;
    }
    for (size_t i = 0; i < strlen(idx); i++){
        if (!isdigit(idx[i])){
            return NULL;
        }
    }
    struct mtll* cursor = head;
    while(cursor != NULL){
        if (cursor->index == atoi(idx)){
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;

}

int mtll_valid_node_idx(char* idx){
    if (!isdigit(idx[0]) && idx[0] != '-' && idx[0] != '+'){
        return 0;
    }
    for (size_t i = 1; i < strlen(idx); i++){
        if (!isdigit(idx[i])){
            return 0;
        }
    }
    return 1;
}

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
