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

char *realloc_string(String *s);

int read_input(FILE* in, String* s, int* iter);

#endif
