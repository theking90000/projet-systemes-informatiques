#ifndef MATH_STRING_H
#define MATH_STRING_H

#include <stdio.h>

typedef struct  m_string {

        char    *ptr;
        size_t  max_size;

}               m_string;

int alloc_string(m_string *s);

/* Check if indice i is accessible (< max_size-1) or reallocate until it is.  */
int string_check(m_string *s,
                 size_t i);

char *realloc_string(m_string *s);

void zero_string(m_string* s);

void free_string(m_string *string);

void copy_string(m_string *s,
                 m_string *d);

int string_check(m_string *s,
                 size_t i);

int increment(m_string *s, size_t begin, size_t *end);

#endif