#ifndef LIST_H
#define LIST_H



typedef struct list_node {

        struct list_node*  next;
        void*              value;

}              list_node;

typedef struct      list {

        list_node*  head;

}                   list;

void init_list (list*     l);

int  push_back (list*     l,
                void* value);

void clear_list(list*     l);

#endif

