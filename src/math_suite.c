#include "math_suite.h"
#include <unistd.h>
#include <stdlib.h>

#define DEFAULT_SIZE 32

int alloc_string(String *string) {
    string->ptr = calloc(DEFAULT_SIZE, sizeof(char));
    if (string->ptr == NULL)
        return -1;
    string->max_size = DEFAULT_SIZE;
    return 0;
}

int realloc_string(String *string) {
    string->max_size *= 2;
    string->ptr = reallocarray(string->ptr, string->max_size, sizeof(char));
    memset(string->ptr, 0, string->max_size);
    if (string->ptr == NULL)
        return -1;
    return 0;
}

int solve(FILE* in, FILE* out, int only_longest, int debug) {
    if(debug>=2) printf("Calcul tr�s long en cours\n");
    sleep(2);
    return 0;
}
