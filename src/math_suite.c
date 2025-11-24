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

void free_string(String *string) {
    free(string->ptr);
    string->ptr = NULL;
}

void* zero_string(String* s) {
    memset(s->ptr, 0, s->max_size);
}

/**
 * Copie le contenu de la chaine s (source) dans la chaine d (destination)
 * Réalloue la chaine d si elle est trop petite.
 */
void copy_string(String *s, String *d) {
    zero_string(d);
    char* sp = s->ptr;
    char* dp = d->ptr;
    while (*sp != '\0') {
        if (dp - d->ptr >= d->max_size)
            dp = realloc_string(d);
        *dp = *sp;
        sp++;
        dp++;
    }
}

/**
 * Incremente le compteur dans la string qui commence à l'endroit *pos
 * Ex: 345 -> 346, 399 -> 400, 9 -> 10
 * Retourne la position de fin du compteur dans la string
 * Et peut changer la valeur de *pos en cas de realloc
 *
 * Entrée:
 *
 * s: |1|1|9|9|9|X|X|X|X
 *         ^
 *        pos
 *
 * Sortie:
 *
 * s: |1|1|1|0|0|0|X|X|X|X
 *         ^     ^
 *        pos   fin
 */
char* increment(String *s, char** pos) {
    if (*pos - s->ptr  >= s->max_size) {
        *pos = realloc_string(s);
    }

    if(**pos == '\0')
        **pos = '0';

    // TODO: gérer la retenue

    **pos += 1;

    return *pos;
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
    zero_string(s);

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
    String  s, s_new;
    int     iter, i;
    char   *s_ptr, *s_new_end, *s_new_start;

    /*
     * s_new_end et s_new_start
     *
     * s_new:
     * |X|X|X|X|X|X|X|
     *  ^
     * start
     * end
     *
     * input=1112
     * s_new:
     * |1|X|X|X|X|X|
     *  ^
     * start
     * end
     * 
     * s_new:
     * |2|X|X|X|X|X|
     *  ^
     * start
     * end
     * 
     * s_new:
     * |3|X|X|X|X|X|
     *  ^
     * start
     * end
     * 
     * s_new (on a lu les 3 '1', on écrit la valeur '1' après):
     * |3|1|X|X|X|X|
     *  ^ ^
     * start
     *   end
     *
     * s_new:
     * |3|1|X|X|X|X|X|
     *      ^
     *    start
     *     end
     *
     * s_new:
     * |3|1|1|X|X|X|X|
     *      ^
     *    start
     *     end
     * s_new:
     * |3|1|1|2|X|X|X|
     *      ^ ^
     *    start
     *       end
     */

    // if(debug>=2) 
    //    printf("I will solve the file {} and write to {}, only longest {}\n", );
    
    alloc_string(&s);
    if(!only_longest) {
        alloc_string(&s_new);
    }

    while(read_input(in, &s, &iter) == 0) {
        if (debug >= 1)
            printf("Solving %d iterations of the value %s\n", iter, s.ptr);

        for(i = 0; i < iter; i++) {
            if (debug >= 1) 
                printf("Iteration %d\n", i);

            s_ptr = s.ptr;
            s_new_end = s_new.ptr;
 
            while(*s_ptr != '\0') {
                // Compter le nombre de '*s_ptr' identiques
                s_new_start = s_new_end;
                //printf("Nombre actuel a change (%c) str=%s |s_new=%s\n", *s_ptr, s.ptr, s_new.ptr);
                s_new_end = increment(&s_new, &s_new_start);
                while (*s_ptr != '\0' && *s_ptr == *(++s_ptr)) {
                    // Incrémenter la case s_new_ptr;
                    s_new_end = increment(&s_new, &s_new_start);
                }
                *(++s_new_end) = *(s_ptr-1);
                s_new_end++;
            }
            
            printf("Iter %d : s_new=%s\n", i, s_new.ptr);
            copy_string(&s_new, &s);
            zero_string(&s_new);
            
        }

        if (only_longest) {
            // Comparer avec List
        } else {
            printf("s_new=%s\n",s_new.ptr);
            zero_string(&s_new);
        }
    }

    free_string(&s);

    //sleep(2);
    return 0;
}
