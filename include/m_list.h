#ifndef M_LIST_H
#define M_LIST_H

#include "m_string.h"

typedef struct m_list_node {

        struct m_list_node*  next;
        m_string              value;

}              m_list_node;

typedef struct      m_list {

        m_list_node*  head;

}                   m_list;

void init_list (m_list*     l);

int  push_back (m_list*     l,
                m_string   value);

void clear_list(m_list*     l);

void print_list(FILE*      out,
                m_list*     l);

#endif

