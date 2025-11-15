#include "led.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26
#define SUPPORTS_HARDWARE_PWM 0

/*
 * TODO: penser a reorganiser comme suit:
 * Puisque qu il faut allumer la LED en rouge en cas d erreur,
 * je propose de creer une fonction void exit_err(Led*);
 * Cependant c est compliqu� car il faut idealement fermer toutes les 
 * resources (in, out, led) avant de quitter.
 *
 * Il me semble que les programmes utilisent un "goto" cleanup qui s'occupe de tout fermer
 * mais a voir comment on fait. Car un exit(1) trop brutal ne laisse pas fermer les resources
 * ouvertes. (donc soit exit_err(Led*, FILE*, FILE*) qui ferme tout) soit un "goto" cleanup.
 * */

int main(int argc, char *argv[]) {
    /* Paramètres du programm: pourra �ventuellement faire l'objet du structure d�diéavec une fonction
     * int parse_args(args*) */
    char     input[255] = {0}; /* --input <path> (optionel)  */
    char     output[255] = {0}; /* --output <path> (optionel) */
    int      only_longest = 0; /* --only-longest (optionnel) */
    int      debug = 0; /* --debug <level> (optionnel) */

    /* Variables utilisées par le programme */
    Led      led;
    int      i;
    FILE*    in  = 0;
    FILE*    out = 0;

    /* Code de retour */
    int      ret = 0;

    /* Détecter les paramètres passés en argument du programme */
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0) {
            if (i+1 >= argc) {
                fprintf(stderr, "Mauvais usage: --input attend un argument\n");
                exit(1);
            }

            strncpy(input, argv[++i], 255);
        }

        if(strcmp(argv[i], "--output") == 0) {
           if(i+1 >= argc) {
               fprintf(stderr, "Mauvais usage: --output attend un argument\n");
               exit(1);
           }

            strncpy(output, argv[++i], 255);
        }

        if (strcmp(argv[i], "--only-longest") == 0) {
            only_longest = 1;
        }

        if (strcmp(argv[i], "--debug") == 0) {
            if(i+1 >= argc) {
                fprintf(stderr, "Mauvais usage: --debug attend un argument\n");
                exit(1);
            }

            // atoi, strtol ne retourne pas si il y a eu une erreur (juste 0)
            // debug = strtol(argv[i+1], NULL, 10);

            if (sscanf(argv[++i], "%d", &debug) == 0) {
               fprintf(stderr, "Mauvais usage: --debug attend un argument entier\n");
               exit(1);
            }
        }
    }

    if (debug >= 3) {
        printf("Debug: parametres du programme\n");
        printf(" - input='%s'\n", input);
        printf(" - output='%s'\n", output);
        printf(" - only_longest=%b\n", only_longest);
        printf(" - debug=%d\n", debug);
    }

    // %--------------------%
    // | Début du programme |
    // %--------------------%


    if (debug >= 3) printf("Debug: initialisation de la LED\n");

    /* Initialiser la LED => peut-être g�rer les erreur? */
    if(init_led(&led, RED_PIN, GREEN_PIN, BLUE_PIN, POWER_PIN,
                !SUPPORTS_HARDWARE_PWM) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de la LED\n");
        exit(1);
    }

    if (strlen(input) == 0) {
        in = stdin;
    } else {
        if (debug >= 3) printf("Debug: ouverture de %s\n", input);

        if((in = fopen(input, "r")) == NULL) {
            printf("Erreur: impossible de lire le fichier %s\n", input);
            /* Affichage de l'erreur en utilisant errno */
            printf("(%s)\n", strerror(errno));
            goto fail;
        }
    }

    if (strlen(output) == 0) {
        out = stdout;
    } else {
        if(debug >= 3) printf("Debug: ouverture de %s en écriture\n", output);

        if ((out=fopen(output, "w")) == NULL) {
            fprintf(stderr, "Erreur: impossible d'écrire le fichier %s\n", output);
            fprintf(stderr, "(%s)\n", strerror(errno));
            goto fail;
        }
    }

    /* Executer la fonction solve() ?*/

    // %============================================%
    // | Fin du programme - Fermeture des resources |
    // %============================================%
    
    // Execution OK => Mettre la LED en vert
    set_color(&led, GREEN);

    // %%%% Label Cleanup pour permettre de goto (jump) directement si necessaire %%%%
    cleanup:
    // turn_off(&led) : ne pas faire ca si on veut laisser la LED allumé
    // Risque de memory LEAK donc a voir si on laisse juste un getchar() dans le vide
    // Ou on coupe le programme => poser la question au prof.
    
    if(in && in != stdin) {
        if(debug >= 3) printf("Debug: fermeture du fichier %s\n", input);

        if(fclose(in) != 0) {
            fprintf(stderr, "Erreur: impossible de fermer le fichier %s\n", input);
            /* Affichage de l'erreur en utilisant errno */
            fprintf(stderr, "(%s)\n", strerror(errno));
        }
    }


    if(out && out != stdout) {
        if(debug >= 3) printf("Debug: fermeture du fichier %s\n", output);

        if(fclose(in) != 0) {
            fprintf(stderr, "Erreur: impossible de fermer le fichier %s\n", output);
            /* Affichage de l'erreur en utilisant errno */
            fprintf(stderr, "(%s)\n", strerror(errno));
        }
    }

    // getchar() ou timeout puis?
    // turn_off(&led) ? 
    
    return ret;

    // Point pour indiquer une erreur
    fail:
        // Mettre la LED en rouge
        // TODO: gerer le cas ou l'erreur est generee par l'initalisation
        // de la LED, dans ce cas on peut pas mettre la led en rouge car 
        // la led n'est pas initialisee justement
        // Rm: normalement avec un early exit apres init_led c'est bon
        set_color(&led, RED);

        // Aller a la sortie: cleanup
        ret = 1;
        goto cleanup;

}
