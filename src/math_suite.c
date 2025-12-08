#include "math_suite.h"
#include "cstring.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Lis une ligne de l'entrée in,
 * Stocke le premier itéré dans la String s (s doit être alloué).
 * Si le 1er itéré est trop grand, réalloue la string s.
 * Stocke le nombre d'itération dans iter.
 */
int read_input(FILE* in, String* s, int* iter) {
    int    c; /* Caractère lu à la position du curseur dans in */
    size_t i; /* Position actuelle dans la String s */

    i=0;

    // S'assurer que la string est vide.
    zero_string(s);

    while(1) {
        c = fgetc(in);

        if (c == EOF) 
            return -1;

        if (c == ' ')
            break;

        string_check(s, i);

        s->ptr[i] = c;
        i++;
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
 
            while(j < s.max_size && s.ptr[j] != '\0') {
                // Compter le nombre de '*s_ptr' identiques
                start = end;
                //printf("Nombre actuel a change (%c) str=%s |s_new=%s\n", s.ptr[j], s.ptr, s_new.ptr);
                if(increment(&s_new, start, &end) == -1)
                    return -1;
                while (++j < s.max_size && s.ptr[j] != '\0' && s.ptr[j-1] == s.ptr[j]) {
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
           // printf("end=%d\n",end);
            //fwrite(s_new.ptr, sizeof(char), end, out);
            //fprintf(out, )
            // fwrite('\n', sizeof(char), 1, out);
            fprintf(out, "%s\n", s.ptr);
            // printf("s_new=%s\n",s_new.ptr);
            zero_string(&s_new);
        }
    }

    free_string(&s);
    if(!only_longest) {
        free_string(&s_new);
    }

    //sleep(2);
    return 0;
}
