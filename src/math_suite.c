#include "math_suite.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_SIZE 32

/**
 * Alloue une nouvelle chaine de caractère (par défaut de taille maximale 32)
 * Retourne 0 si tout c'est bien passé ou -1 si une erreur.
 */
int alloc_string(String *string) {
    string->ptr = calloc(DEFAULT_SIZE, sizeof(char));

    if (string->ptr == NULL)
        return -1;

    string->max_size = DEFAULT_SIZE;
    return 0;
}

/**
 * Duplique la taille de la chaine de caractère
 * Retourne un pointeur sur la moitié de la chaine de caractère.
 * ou NULL si une erreur.
 */
char* realloc_string(String *string) {
    string->ptr = realloc(string->ptr, string->max_size*2 * sizeof(char));
    
    if (string->ptr == NULL)
        return NULL;

    memset(string->ptr + string->max_size, 0, string->max_size);
    string->max_size *= 2;

    return string->ptr + (string->max_size/2);
}

/**
 * Lis une ligne de l'entrée in,
 * Stocke le premier itéré dans la String s (s doit être alloué).
 * Si le 1er itéré est trop grand, réalloue la string s.
 * Stocke le nombre d'itération dans iter.
 */
int read_input(FILE* in, String* s, int* iter) {
    int c; /* Caractère lu à la position du curseur dans in */
    char* s_ptr; /* Position actuelle dans la String s */

    s_ptr = s->ptr;

    // S'assurer que la string est vide.
    memset(s->ptr, 0, s->max_size);

    while(1) {
        c = fgetc(in);

        if (c == EOF) 
            return -1;

        if (c == ' ')
            break;

        if (s_ptr - s->ptr >= s->max_size) {
            s_ptr = realloc_string(s);
            if (s_ptr == NULL)
                return -1;
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
    String  s;
    int     iter;

    // if(debug>=2) 
    //    printf("I will solve the file {} and write to {}, only longest {}\n", );
    
    alloc_string(&s);

    while(read_input(in, &s, &iter) == 0) {
        printf("Lecture ligne s=%s iter=%d\n", s.ptr, iter);
    }

    //sleep(2);
    return 0;
}
