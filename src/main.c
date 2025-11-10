#include "led.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26

int main(int argc, char *argv[]) {
    /* Paramètres du programme */
    char     input[255] = {0}; /* --input <path> (optionel)  */
    char     output[255] = {0}; /* --output <path> (optionel) */
    int      only_longest = 0; /* --only-longest (optionnel) */
    int      debug = 0; /* --debug <level> (optionnel) */

    /* Variables utilisées par le programme */
    Led      led;
    int      i;
    FILE*    in;
    FILE*    out;

    /* Détecter les paramètres passés en argument du programme */
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0) {
            if (i+1 >= argc) {
                printf("Mauvais usage: --input attend un argument\n");
                exit(1);
            }

            strncpy(input, argv[++i], 255);
        }

        if(strcmp(argv[i], "--output") == 0) {
           if(i+1 >= argc) {
               printf("Mauvais usage: --output attend un argument\n");
               exit(1);
           }

            strncpy(output, argv[++i], 255);
        }

        if (strcmp(argv[i], "--only-longest") == 0) {
            only_longest = 1;
        }

        if (strcmp(argv[i], "--debug") == 0) {
            if(i+1 >= argc) {
                printf("Mauvais usage: --debug attend un argument\n");
                exit(1);
            }

            // atoi, strtol ne retourne pas si il y a eu une erreur (juste 0)
            // debug = strtol(argv[i+1], NULL, 10);

            if (sscanf(argv[++i], "%d", &debug) == 0) {
               printf("Mauvais usage: --debug attend un argument entier\n");
               exit(1);
            }
        }
    }

    if (debug >= 3) {
        printf("Debug Params\n");
        printf(" - input='%s'\n", input);
        printf(" - output='%s'\n", output);
        printf(" - only_longest=%b\n", only_longest);
        printf(" - debug=%d\n", debug);
    }

    // %--------------------%
    // | Début du programme |
    // %--------------------%

    if (strlen(input) == 0) {
        in = stdin;
    } else {
        if(fopen(input, "r") == NULL) {
            printf("Erreur: impossible de lire le fichier %s\n", input);
            /* Affichage de l'erreur en utilisant errno */
            printf("(%s)\n", strerror(errno));
            exit(1);
        }
    }

    if (strlen(output) == 0) {
        out = stdout;
    } else {
        if (fopen(input, "w") == NULL) {
            printf("Erreur: impossible d'�crire le fichier %s\n", output);
            printf("(%s)\n", strerror(errno));
            exit(1);
        }
    }

    /* Initialiser la LED => peut-être g�rer les erreur? */
    init_led(&led, RED_PIN, GREEN_PIN, BLUE_PIN, POWER_PIN);

    /* Executer la fonction solve() ?*/

    // %============================================%
    // | Fin du programme - Fermeture des resources |
    // %============================================%

    if (strlen(input) != 0) {
        fclose(in);
    } 

    if (strlen(output) != 0) {
        fclose(out);
    }

    return 0;
}
