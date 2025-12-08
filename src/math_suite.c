#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "math_suite.h"
#include "cstring.h"
#include "list.h"

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

    // Pour le only_longest
    list        longest;
    compare_key longest_key;


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
     //   printf("I will solve the file {} and write to {}, only longest {}\n", );
    
    if (alloc_string(&s) == -1)
        return -1;
    

    if(alloc_string(&s_new) == -1)
        return -1;

    if (only_longest) {
        init_list(&longest);
        get_compare_key(&longest_key, s);
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
            // Ajouter pour test
            switch (compare(&longest_key, s)) {
                case 1:
                    clear_list(&longest);
                case 0:
                    push_back(&longest, s);
            }
            // printf("list\n");
            // print_list(out, &longest);
            alloc_string(&s);
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
    } else {
        // Affiche contenu
        print_list(out, &longest);
        clear_list(&longest);
    }

    //sleep(2);
    return 0;
}

void get_compare_key(compare_key* k, String s) {
    // char n[10] = {0};
    
    // Chaque bit de 0 a 9 de n
    // Indique la presence du nombre;
    short n;
    int   i;
    
    for (i = 0; s.ptr[i] != '\0'; i++) {
        n |= (1 << (s.ptr[i] - '0'));
    }
    
    k->length = i;
    k->ndiff  = 0;

    for (i = 0; i < 10; i++) {
        if (((n>>i) & 1) == 1) {
            k->ndiff++;
        }
    }
}

int compare_result(compare_key k1, compare_key k2) {
    if (k1.length == k2.length) {
        if (k1.ndiff == k2.ndiff) return 0;
        if (k1.ndiff > k2.ndiff) return 1;
        return -1;
    }

    if (k1.length > k2.length) return 1;
    return -1;
}

int compare(compare_key* k, String s) {
    compare_key k2;
    int        cmp;

    get_compare_key(&k2, s);
    
    //printf("CompareKey k : %d %d\n", k->length,k->ndiff);
    //printf("COmpareKEY k2: %d, %d\n", k2.length, k2.ndiff);
    if (k == NULL || (cmp = compare_result(k2, *k)) == 1) {
        *k = k2;
        return 1;
    }
    
    return cmp;
}
