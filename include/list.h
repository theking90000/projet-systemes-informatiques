#ifndef LIST_H
#define LIST_H

#include "cstring.h"

typedef struct list_node {

        struct list_node*  next;
        String              value;

}              list_node;

typedef struct      list {

        list_node*  head;

}                   list;

void init_list (list*     l);

int  push_back (list*     l,
                String   value);

void clear_list(list*     l);

void print_list(FILE*     f,
                list*     l);

#endif

