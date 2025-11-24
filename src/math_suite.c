#include "math_suite.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_SIZE 32

int alloc_string(String *string) {
    string->ptr = calloc(DEFAULT_SIZE, sizeof(char));
    if (string->ptr == NULL)
        return -1;
    string->max_size = DEFAULT_SIZE;
    return 0;
}

int realloc_string(String *string) {
    string->ptr = realloc(string->ptr, string->max_size*2 * sizeof(char));
    if (string->ptr == NULL)
        return -1;
    memset(string->ptr + string->max_size, 0, string->max_size);
    string->max_size *= 2;
    return 0;
}

int read_input(FILE* in, String* s, int* iter) {
    char c;
    char* s_ptr;

    s_ptr = s->ptr;

    while(1) {
        c = fgetc(in);

        if (c == EOF) 
            return -1;

        if (c == ' ')
            break;

        if (s_ptr - s->ptr >= s->max_size) {
            // TODO
        }

        *s_ptr = c;
        s_ptr++;
    }

    *iter = 0;

    while(1) {
        c = fgetc(in); 

        if (c == EOF) 
            return -1;

        if (c == '\n')
            break;

        *iter *= 10;
        *iter += c - '0';
    }

    return 0;
}

int solve(FILE* in, FILE* out, int only_longest, int debug) {
    if(debug>=2) printf("Calcul tr�s long en cours\n");
    sleep(2);
    return 0;
}
