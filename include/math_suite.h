#ifndef MATH_SUITE
#define MATH_SUITE

#include <stdio.h>
#include "m_string.h"

typedef struct compare_key {
            int length;
            int ndiff ;
}              compare_key;

int solve(FILE*   in, 
          FILE*   out, 
          int     only_longest, 
          int     debug);

int read_input(FILE* in, m_string* s, int* iter);

// Comparer 2 "Compare Key"
// Retourne 
//   0 si egal
//   1 si k1 > k2
//  -1 si k1 < k2
int compare_result(compare_key k1, compare_key k2);

void get_compare_key(compare_key* k, m_string s);

// Comparer la cléactuelle de comparaison
// (NULL si pas de cl�)
// Avec la string 's'
// Si la string key('s') > 'k' retourne 1 et stocke la nouvelle cle dans 'k'
// Si key('s') < 'k' retourne -1 et ne fait rien
// Si egal retourne 0
int compare(compare_key* k, m_string s);

#endif
