#include <stdlib.h>
#include "list.h"

void init_list(list* l) {
    l->head = NULL;
}

int push_back(list* l, String value) {
    int         pos;
    list_node* curr;
    list_node* node;

    curr = l->head;
    pos  = 0;
    node = malloc(sizeof(list_node));

    if(node == NULL)
        return -1;

    node->next = NULL;
    node->value = value;
    
    while(curr != NULL && curr->next != NULL) {
        curr = curr->next;
        pos++;
    }

    if(curr == NULL) {
        l->head = node;
    } else {
        pos++;
        curr->next = node;
    }

    return pos;
}

void clear_list(list* l) {
    list_node* curr;
    list_node* next;
    
    curr = l->head;

    while(curr != NULL) {
        next = curr->next;
        free_string(&curr->value);
        free(curr);
        curr = next;
    }

    l->head = NULL;
}

void print_list(FILE* out, list* l) {
    list_node* curr;

    curr = l->head;
    
    while(curr != NULL) {
        fprintf(out, "%s\n", curr->value.ptr);
        curr = curr->next;
    }
}
