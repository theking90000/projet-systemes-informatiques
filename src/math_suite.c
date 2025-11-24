#include "math_suite.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_SIZE 2

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
 * Incremente le compteur dans la string qui commence à l'endroit *begin
 * et se termine à l'endroid *end.
 * La fonction suppose que la string vaut '\0' après end et réallouera si nécessaire.
 * Si une retenu est nécessaire '9' la string sera décalée vers la droite
 * |9|9|9|X|X| -> |1|0|0|0|X|
 * 
 * Ex: 345 -> 346, 399 -> 400, 9 -> 10
 * Les valeur de *start et de *end
 * peuvent changer en cas de realloc ou d'overflow.
 *
 * Entrée:
 *
 * s: |1|1|9|9|9|X|X|X|X
 *         ^   ^
 *       start end
 *
 * Sortie:
 *
 * s: |1|1|1|0|0|0|X|X|X|X
 *         ^     ^
 *       start  end
 */
void increment(String *s, char** begin, char** end) {
    char *p1, *p2, *p3;

    // Pas assez de place pour écrire à begin
    // : augmenter la taille.
    if (*begin - s->ptr  >= s->max_size) {
        p1 = s->ptr;
        *begin = realloc_string(s);
        printf("REALLOC_BEGIN! %d\n", (s->ptr-p1));
        // Ajouter la distance entre (s->ptr avant et s->ptr après le realloc)
        // Car tout bouge de la même manière.
        p1 = (s->ptr - p1);
        *end += (size_t)p1;
    }

    // Si begin n'a jamais étée incrementée.
    // Mettre sa valeur à zéro.
    if(**begin == '\0')
        **begin = '0';

    printf("Increment %s %d-%d\n", s->ptr, (*begin-s->ptr), (*end-s->ptr));

    for(p1 = *end; p1 > **begin; p1--) {
        if (*p1 == '9') {
            // Algorithme de report
            // Regarder si il y a un chiffre disponible à gauche (pos-1) >= *begin
            // Si oui reporter a ce chiffre.
            // Sinon poser un '1', décaler tout vers la droit et mettre un zéro.
            
            // On ne reporte pas
            if (p1-1 < *begin) {
                *p1 = '1';
                // Pour l'instant
                // décaler tout ici. et mettre un zéro
                p1++;
                
                p2 = *end;

                // Avant de shifter vers la droite.
                // Vérifier si il y l'espace disponible pour le faire, sinon réallouer.
                if (p2+1 - s->ptr >= s->max_size) {
                    printf("REALLOC!\n");
                    p3 = s->ptr;
                    // Il faut repositionner p2, end, begin, p1 correctement
                    p2 = realloc_string(s);
                    p3 = s->ptr - p3;
                    printf("String a bougé de %d\n",p3);
                    *end += (size_t)p3;
                    *begin += (size_t)p3;
                    p1 += (size_t)p3; 
                }

                while(p2 >= p1) {
                    // Décaler end vers end+1;
                    // TODO: Realloc
                    *(p2+1) = *p2;
                    p2--;
                }
                *(p1) = '0';
                
                *end += 1;
                // printf("IncEND %d\n",(*end - s->ptr));
                // Décaler vers la droite.
                break;
            }

            *p1 = '0';
            // ça sera pour le suivant?
        } else {
            *p1 += 1;
            break;
        }
    }
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
                increment(&s_new, &s_new_start, &s_new_end);
                while (*s_ptr != '\0' && *s_ptr == *(++s_ptr)) {
                    // Incrémenter la case s_new_ptr;
                    increment(&s_new, &s_new_start, &s_new_end);
                }
                // TODO: check realloc.
                *(++s_new_end) = *(s_ptr-1);
                s_new_end++;
            }
            
            if(debug >= 2)
                printf("Iterating over %s gives %s\n", s.ptr, s_new.ptr);
        
            copy_string(&s_new, &s);
            zero_string(&s_new);
            // Pour tester
            free_string(&s_new); alloc_string(&s_new);
        }

        if (debug >= 1)
            printf("Result: %s\n", s.ptr);

        if (only_longest) {
            // Comparer avec List
        } else {
            // Write to output
            fprintf(out, "%s\n", s.ptr);
            // printf("s_new=%s\n",s_new.ptr);
            zero_string(&s_new);
        }
    }

    free_string(&s);

    //sleep(2);
    return 0;
}
