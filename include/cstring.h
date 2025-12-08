#ifndef CSTRING_H
#define CSTRING_H

#include <stdio.h>

typedef struct  String {

        char    *ptr;
        size_t  max_size;

}               String;

int alloc_string(String *s);

/* Check if indice i is accessible (< max_size-1) or reallocate until it is.  */
int string_check(String *s,
                 size_t i);

char *realloc_string(String *s);

void zero_string(String* s);

void free_string(String *string);

void copy_string(String *s,
                 String *d);

int string_check(String *s,
                 size_t i);

int increment(String *s, size_t begin, size_t *end);

#endif