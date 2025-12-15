#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "math_suite.h"
#include "m_string.h"
#include "m_list.h"



/**
 * Lis une ligne de l'entrée in,
 * Stocke le premier itéré dans la m_string s (s doit être alloué).
 * Si le 1er itéré est trop grand, réalloue la string s.
 * Stocke le nombre d'itération dans iter.
 */
int read_input(FILE* in, m_string* s, int* iter) {
    int    c; /* Caractère lu à la position du curseur dans in */
    size_t i; /* Position actuelle dans la m_string s */

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

int solve(FILE* in, FILE* out, int only_longest, int debug
          #ifdef LED_SOLVE
          ,Led*    led
          #endif
         ) {
    int ret; // Code de retour (pour la gestion des erreurs)

    int     iter, i;
    size_t  j, start, end;

    m_string  s     = {0};
    m_string  s_new = {0};

    // Pour le only_longest
    m_list          longest = {0};
    compare_key longest_key = {0};

    #ifdef LED_SOLVE
    long long led_iter   = 0;
    #endif

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
    
    // TODO: /!\ Return -1, d'abord free toutes les res.
    if (alloc_string(&s) == -1) {
        ret = -1; goto clean;
    }
    
    if(alloc_string(&s_new) == -1) {
        ret = -1; goto clean;
    }

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
                if(increment(&s_new, start, &end) == -1) {
                    // return -1;
                    ret = -1; goto clean;
                }
                while (++j < s.max_size && s.ptr[j] != '\0' && s.ptr[j-1] == s.ptr[j]) {
                    // Incrémenter la case s_new_ptr;
                    //printf("inc\n");
                    if(increment(&s_new, start, &end) == -1) {
                        ret = -1; goto clean;
                    }
                }
                //printf("--------------------------------------\n%s\n---------------\n",s_new.ptr);
                if (string_check(&s_new, end+1) == -1) {
                    ret = -1; goto clean;
                }
                s_new.ptr[++end] = s.ptr[j-1];
                end++;
                
                #ifdef LED_SOLVE
                led_iter++;
                if (led_iter % 10000000 == 0) {
                    //printf("C=BLUE %d\n", led_iter);
                    set_color(led, BLUE);
                } else if (led_iter % 10000000 == 5000000) {
                    //printf("C=WHITE %d\n", led_iter);
                    set_color(led, WHITE);
                }
                #endif
            }
            
            if(debug >= 2)
                printf("Iterating over %s gives %s\n", s.ptr, s_new.ptr);
            
            swap_string(&s, &s_new);
            //printf("After swap: s=%s | s_new=%s\n", s.ptr, s_new.ptr);
            //copy_string(&s_new, &s);

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
                    /* fallthrough */
                case 0:
                    push_back(&longest, s);
                    
                    if(alloc_string(&s) == -1) {
                        ret = -1; goto clean;
                    }
            }
            // printf("list\n");
            // print_list(out, &longest);
        } else {
            
            // Write to output
           // printf("end=%d\n",end);
            //fwrite(s_new.ptr, sizeof(char), end, out);
            //fprintf(out, )
            // fwrite('\n', sizeof(char), 1, out);
            // fprintf(out, "%s\n", s.ptr);

            // Etre sur que libc n'a pas de buffer en attente
            fflush(out);
            print_string(out, &s, end);
            // printf("s_new=%s\n",s_new.ptr);
            zero_string(&s_new);
        }
    }

    if(only_longest) {
        // Affiche contenu
        fflush(out);
        print_list(out, &longest);
    }

    ret = 0;

    clean:
        free_string(&s);
        free_string(&s_new);

        if (only_longest) {
            clear_list(&longest);
        }

    return ret;
}

void get_compare_key(compare_key* k, m_string s) {
    // char n[10] = {0};
    
    // Chaque bit de 0 a 9 de n
    // Indique la presence du nombre;
    short n;
    int   i;
    
    n = 0;
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

int compare(compare_key* k, m_string s) {
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
