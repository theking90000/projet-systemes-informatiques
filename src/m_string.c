#include "m_string.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 32

/**
 * Alloue une nouvelle chaine de caractère (par défaut de taille maximale 32)
 * Retourne 0 si tout c'est bien passé ou -1 si une erreur.
 */
int alloc_string(m_string *string) {
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
char* realloc_string(m_string *string) {
    string->ptr = realloc(string->ptr, string->max_size*2 * sizeof(char));
    
    if (string->ptr == NULL)
        return NULL;

    memset(string->ptr + string->max_size, 0, string->max_size);
    string->max_size *= 2;

    return string->ptr + (string->max_size/2);
}

void free_string(m_string *string) {
    if (string->ptr != NULL)
        free(string->ptr);
    
    string->ptr = NULL;
}

void zero_string(m_string* s) {
    memset(s->ptr, 0, s->max_size);
}

/**
 * Copie le contenu de la chaine s (source) dans la chaine d (destination)
 * Réalloue la chaine d si elle est trop petite.
 */
void copy_string(m_string *s, m_string *d) {
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

int string_check(m_string *s, size_t i) {
    //printf("m_string check i=%d, max_size=%d\n", i, s->max_size);
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
int increment(m_string *s, size_t begin, size_t *end) {
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