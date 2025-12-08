#include "led.h"
#include "math_suite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26
#define PWM_TYPE NO_PWM

#if USE_FORK

/* Variable indiquant si le child tourne ou pas 
 * volatile : force le compilateur a reverifier la valeur en memoire a chaque fois
 *            et evite les optimisations du style cette valeur ne change jamais
 * sig_atomic_t : type de donnees qui peut etre modifier dans un signal handler */
volatile sig_atomic_t    running;

void sig_handler() {
    // Informer que le SIGCHILD recu => le child ne tourne plus.
    // printf("Signal recu!!\n");
    running = 0;
}

#endif

int main(int argc, char *argv[]) {
    /* Paramètres du programm: pourra �ventuellement faire l'objet du structure d�diéavec une fonction
     * int parse_args(args*) */
    char     input[256] = {0}; /* --input <path> (optionel)  */
    char     output[256] = {0}; /* --output <path> (optionel) */
    int      only_longest = 0; /* --only-longest (optionnel) */
    int      debug = 0; /* --debug <level> (optionnel) */

    /* Variables utilisées par le programme */
    Led      led;
    int      i;
    FILE*    in  = 0;
    FILE*    out = 0;

    #if USE_FORK
    pid_t    pid;
    int      status;
    #endif

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
    if(init_led(&led, RED_PIN, GREEN_PIN, BLUE_PIN, POWER_PIN, PWM_TYPE) != 0) {
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

    #if USE_FORK
        if(debug >= 3) printf("Execution du fork\n");

        running = 1;
        signal(SIGCHLD, sig_handler);

        pid = fork();

        if (pid == -1) {
            fprintf(stderr, "Erreur: impossible de fork()\n");
            goto fail;
        }
    
        // Dans le child
        if(pid == 0) {
            if(debug >= 3) printf("Child pid=%d, ppid=%d\n", getpid(), getppid());
            ret = solve(in, out, only_longest, debug);
            // Meme dans le child, il faut fermer les fd pour contenter valgrind
            // Meme si en realite les open file objects restent ouvert tant qu'il y 
            // a le parent ou l'enfant qui possede encore un fd pas fermé.
            goto cleanup;
        } else {
            if(debug >= 3) printf("Parent pid=%d\n", getpid());
        }
    #endif

    #if SINGLE
        solve(in, out, only_longest, debug);
    
    // Ici on est d'office dans le parent
    // -> Faire clignoter la LED.
    #else
    #if USE_FORK
    while(running) {
    #endif
        if(debug>=3) printf("child tourne encore\n");

        set_color(&led, BLUE);

        // Sleep pendant 500ms
        if(!running || (usleep(500 * 1000) == -1 && errno == EINTR)) {
            if(debug>=3) printf("sleep interrompu\n");
            break;
        };

        set_color(&led, WHITE);

        // Sleep pendant 500ms
        if(!running || (usleep(500 * 1000) == -1 && errno == EINTR)) {
            if(debug>=3) printf("sleep interrompu\n");
            break;
        }
    }
    
    #if USE_FORK
    if(waitpid(pid, &status, 0) == -1) {
        perror("wait(): erreur\n");
        goto fail;
    } else {
        if (debug >= 3) printf("child a quitte avec %d\n", status);

        if (status != 0) {
            goto fail;
        }
    }
    #endif
    #endif

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

        if(fclose(out) != 0) {
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
        if (debug>=3) printf("Fail \n");
        
        set_color(&led, RED);

        // Aller a la sortie: cleanup
        ret = 1;
        goto cleanup;

}
