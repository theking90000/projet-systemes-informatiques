#ifndef MATH_SUITE
#define MATH_SUITE

#include <stdio.h>

typedef struct  String {

        char    *ptr;
        size_t  max_size;

}               String;

int solve(FILE*   in, 
          FILE*   out, 
          int     only_longest, 
          int     debug);

int alloc_string(String *s);

int realloc_string(String *s);

#endif
