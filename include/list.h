#ifndef LIST_H
#define LIST_H

#include "m_string.h"

typedef struct list_node {

        struct list_node*  next;
        m_string              value;

}              list_node;

typedef struct      list {

        list_node*  head;

}                   list;

void init_list (list*     l);

int  push_back (list*     l,
                m_string   value);

void clear_list(list*     l);

void print_list(FILE*     f,
                list*     l);

#endif

