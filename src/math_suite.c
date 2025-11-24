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

void zero_string(String* s) {
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
        if ((size_t)(dp - d->ptr) >= d->max_size)
            dp = realloc_string(d);
        *dp = *sp;
        sp++;
        dp++;
    }
}

int string_check(String *s, size_t i) {
    //printf("String check i=%d, max_size=%d\n", i, s->max_size);
    while (i >= s->max_size - 1) {
        // printf("Realloc max_size=%d | %s \n", s->max_size, s->ptr);
        if(realloc_string(s) == NULL)
            return -1;
        // printf("Done: %d\n",s->max_size, s->ptr);
    }

    return 0;
}

/**
 * Incremente le compteur dans la string qui commence à l'indice begin (inclusif)
 * et se termine à l'indice *end (inclusif) (indices commençant par 0).
 * 
 * *end doit toujours être supérieur ou égal à begin et begin doit être >0.
 * 
 * Retourne -1 si erreur de réallocation.
 * 
 * La fonction suppose que la string vaut '\0' après *end et réallouera si nécessaire.
 * Si une retenu est nécessaire '9' la string sera décalée vers la droite
 * |9|9|9|X|X| -> |1|0|0|0|X|
 * 
 * Ex: 345 -> 346, 399 -> 400, 9 -> 10
 * L'indice *end peut augmenter en cas d'overflow. (start reste toujours au même indice)
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
int increment(String *s, size_t begin, size_t *end) {
    size_t p1, p2;

    if(string_check(s, begin) == -1)
        return -1;

    // Si [begin] n'a jamais étée incrementée.
    // Mettre sa valeur à zéro.
    if(s->ptr[begin] == '\0')
        s->ptr[begin] = '0'; // équivalent a "*(s->ptr + begin)"

    //printf("Increment %s %d-%d\n", s->ptr, begin, *end);

    for(p1 = *end; p1 >= begin; p1--) {
        //printf("p1=%d\n",p1);
        if (s->ptr[p1] == '9') {
            // Algorithme de report
            // Regarder si il y a un chiffre disponible à gauche (pos-1) >= *begin
            // Si oui reporter a ce chiffre.
            // Sinon poser un '1', décaler tout vers la droit et mettre un zéro.
            
            // On ne reporte pas
            if (p1 == begin) {
                s->ptr[p1] = '1';
                // Pour l'instant
                // décaler tout ici. et mettre un zéro
                p1++;
                
                p2 = *end;

                // Avant de shifter vers la droite.
                // Vérifier si il y l'espace disponible pour le faire, sinon réallouer.
                if(string_check(s, p2+1) == -1)
                    return -1;

                while(p2 >= p1) {
                    // Décaler end vers end+1;
                    // TODO: Realloc
                    s->ptr[p2+1] = s->ptr[p2];
                    p2--;
                }
                s->ptr[p1] = '0';
                
                *end += 1;
                // printf("IncEND %d\n",(*end - s->ptr));
                // Décaler vers la droite.
                break;
            }

            s->ptr[p1] = '0';
            // ça sera pour le suivant?
        } else {
            s->ptr[p1] += 1;
            break;
        }
    }

    return 0;
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

        if ((size_t) (s_ptr - s->ptr) >= s->max_size) {
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
    size_t  j, start, end;

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
    
    if (alloc_string(&s) == -1)
        return -1;
    

    if(!only_longest) {
        if(alloc_string(&s_new) == -1)
            return -1;
    }

    while(read_input(in, &s, &iter) == 0) {
        if (debug >= 1)
            printf("Solving %d iterations of the value %s\n", iter, s.ptr);

        for(i = 0; i < iter; i++) {
            if (debug >= 1) 
                printf("Iteration %d\n", i+1);

            j = 0;
            end = 0;
 
            while(s.ptr[j] != '\0') {
                // Compter le nombre de '*s_ptr' identiques
                start = end;
                //printf("Nombre actuel a change (%c) str=%s |s_new=%s\n", s.ptr[j], s.ptr, s_new.ptr);
                if(increment(&s_new, start, &end) == -1)
                    return -1;
                while (s.ptr[j++] != '\0' && s.ptr[j-1] == s.ptr[j]) {
                    // Incrémenter la case s_new_ptr;
                    //printf("inc\n");
                    if(increment(&s_new, start, &end) == -1)
                        return -1;
                }
                //printf("--------------------------------------\n%s\n---------------\n",s_new.ptr);
                if (string_check(&s_new, end+1) == -1)
                    return -1;
                s_new.ptr[++end] = s.ptr[j-1];
                end++;
            }
            
            if(debug >= 2)
                printf("Iterating over %s gives %s\n", s.ptr, s_new.ptr);
        
            copy_string(&s_new, &s);
            zero_string(&s_new);
            // Pour tester
            //free_string(&s_new); alloc_string(&s_new);
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
