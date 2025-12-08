#ifndef MATH_SUITE
#define MATH_SUITE

#include <stdio.h>
#include "cstring.h"

int solve(FILE*   in, 
          FILE*   out, 
          int     only_longest, 
          int     debug);

int read_input(FILE* in, String* s, int* iter);

#endif
