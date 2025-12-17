#define _POSIX_C_SOURCE 200809L

#include "led.h"
#include "math_suite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#if USE_THREAD
    #include <pthread.h>
    #include <time.h>
#elif USE_FORK
    #include <setjmp.h>
    #include <signal.h>
    #include <time.h>
    // On met l'attribut statique pour eviter les warnings
    // variable 'X' might be clobbered by 'longjmp' or 'vfork'
    // ca fonctionne parce que main est appele une seule fois
    #define VARIABLE_ATTR static
#endif

#ifndef VARIABLE_ATTR
    #define VARIABLE_ATTR
#endif

#define POWER_PIN 18
#define RED_PIN 12
#define GREEN_PIN 19
#define BLUE_PIN 13


/* Variable indiquant si le child tourne ou pas 
 * volatile : force le compilateur a reverifier la valeur en memoire a chaque fois
 *            et evite les optimisations du style cette valeur ne change jamais
 * sig_atomic_t : type de donnees qui peut etre modifier dans un signal handler */

// volatile sig_atomic_t    running;

#if USE_FORK
sigjmp_buf env;

void sig_handler() {
    // Informer que le SIGCHILD recu => le child ne tourne plus.
    // printf("Signal recu!!\n");
    // running = 0;
    siglongjmp(env, 1);
}
#elif USE_THREAD
static pthread_mutex_t running_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t running_cond = PTHREAD_COND_INITIALIZER;

struct t_args {
    FILE*   in;
    FILE*   out;
    int     only_longest;
    int     debug;
};

volatile int running;

void* thread_solve(void* args_void) {
    struct t_args* args = (struct t_args*)args_void;

    int ret = solve(args->in, args->out, args->only_longest, args->debug);

    pthread_mutex_lock(&running_lock);
    running = 0;
    pthread_cond_signal(&running_cond);
    pthread_mutex_unlock(&running_lock);

    return (void*) ((long)ret);
}
#endif

int main(int argc, char *argv[]) {
    /* Paramètres du programm: pourra éventuellement faire l'objet du structure dédié avec une fonction
     * int parse_args(args*) */
    char     input[256] = {0}; /* --input <path> (optionel)  */
    char     output[256] = {0}; /* --output <path> (optionel) */
    VARIABLE_ATTR int      only_longest = 0; /* --only-longest (optionnel) */
    int      debug = 0; /* --debug <level> (optionnel) */

    /* Variables utilisées par le programme */
    VARIABLE_ATTR Led      led;
    VARIABLE_ATTR int      i;
    VARIABLE_ATTR FILE*    in  = 0;
    VARIABLE_ATTR FILE*    out = 0;
    #if USE_THREAD
    #endif

    #if USE_FORK
    pid_t           pid;
    int             status;
    struct timespec ts;
    #elif USE_THREAD
    pthread_t       thread;
    struct t_args   thread_args;
    struct timespec ts;
    void*           thread_ret;
    #endif

    /* Code de retour */
    VARIABLE_ATTR int      ret = 0;

    /* Initialiser la LED => peut-être g�rer les erreur? */
    if(init_led(&led, RED_PIN, GREEN_PIN, BLUE_PIN, POWER_PIN) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de la LED\n");
        exit(1);
    }

    /* Détecter les paramètres passés en argument du programme */
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0) {
            if (i+1 >= argc) {
                fprintf(stderr, "Mauvais usage: --input attend un argument\n");
                goto fail;
            }

            strncpy(input, argv[++i], 255);
        }

        if(strcmp(argv[i], "--output") == 0) {
           if(i+1 >= argc) {
              fprintf(stderr, "Mauvais usage: --output attend un argument\n");
              goto fail;
           }

            strncpy(output, argv[++i], 255);
        }

        if (strcmp(argv[i], "--only-longest") == 0) {
            only_longest = 1;
        }

        if (strcmp(argv[i], "--debug") == 0) {
            if(i+1 >= argc) {
                fprintf(stderr, "Mauvais usage: --debug attend un argument\n");
                goto fail;
            }

            // atoi, strtol ne retourne pas si il y a eu une erreur (juste 0)
            // debug = strtol(argv[i+1], NULL, 10);

            if (sscanf(argv[++i], "%d", &debug) == 0) {
                fprintf(stderr, "Mauvais usage: --debug attend un argument entier\n");
                goto fail;
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


    // if (debug >= 3) printf("Debug: initialisation de la LED\n");


    if (strlen(input) == 0) {
        in = stdin;
    } else {
        if (debug >= 3) printf("Debug: ouverture de %s\n", input);

        if((in = fopen(input, "r")) == NULL) {
            //fprintf(stderr, "Erreur: impossible de lire le fichier %s\n", input);
            /* Affichage de l'erreur en utilisant errno */
            //fprintf(stderr, "(%s)\n", strerror(errno));
            perror("Erreur: impossible de lire le fichier");
            goto fail;
        }
    }

    if (strlen(output) == 0) {
        out = stdout;
    } else {
        if(debug >= 3) printf("Debug: ouverture de %s en écriture\n", output);

        if ((out=fopen(output, "w")) == NULL) {
            // fprintf(stderr, "Erreur: impossible d'écrire le fichier %s\n", output);
            // fprintf(stderr, "(%s)\n", strerror(errno));
            perror("Erreur: impossible d'ouvrir le fichier en ecriture");
            goto fail;
        }
    }

    /* Executer la fonction solve() ?*/

    #if USE_FORK
        if(sigsetjmp(env, 1) == 0) {
            if(debug >= 3) printf("Execution du fork\n");

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

                ts.tv_sec = 0;
                ts.tv_nsec = 500 * 1000 * 1000; /* 500ms */

                while(1) {
                    if (debug >= 3) printf("Led Bleu\n");
                    set_color(&led, BLUE);
                    nanosleep(&ts, NULL);
                    if (debug >= 3) printf("Led Blanc\n");
                    set_color(&led, WHITE);
                    nanosleep(&ts, NULL);
                }
            }
        }
    #elif USE_THREAD
        if(debug >= 3) printf("Execution du thread\n");
        thread_args.in = in;
        thread_args.out = out;
        thread_args.only_longest = only_longest;
        thread_args.debug = debug;
        running = 1;
        pthread_create(&thread, NULL, thread_solve, (void*)&thread_args);
    #else
        solve(in, out, only_longest, debug
        #ifdef LED_SOLVE
        , &led
        #endif
        );
    #endif

    // Ici on est d'office dans le parent
    // -> Faire clignoter la LED.

    // Side note: pas de mutex sur la lecture car opération atomique
    // mutex sur écriture pour éviter une écriture concurent
    #if USE_THREAD
    while(running) {
        if(debug>=3) printf("thread tourne encore\n");

        set_color(&led, BLUE);

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += 500 * 1000 * 1000;

        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec += 1;
        }

        pthread_mutex_lock(&running_lock);
        if (pthread_cond_timedwait(&running_cond, &running_lock, &ts) != ETIMEDOUT && !running) {
            if(debug>=3) printf("sleep interrompu\n");
            pthread_mutex_unlock(&running_lock);
            break;
        }

        set_color(&led, WHITE);

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += 500 * 1000 * 1000;

        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec += 1;
        }

        if (pthread_cond_timedwait(&running_cond, &running_lock, &ts) != ETIMEDOUT && !running) {
            if(debug>=3) printf("sleep interrompu\n");
            pthread_mutex_unlock(&running_lock);
            break;
        }
        pthread_mutex_unlock(&running_lock);
    }
    
    #elif USE_FORK
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

    #if USE_THREAD
    if (pthread_join(thread, &thread_ret) != 0) {
        perror("thread(): erreur\n");
        goto fail;
    } else {
        if (thread_ret != 0) {
            goto fail;
        }
    }
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

    #if USE_THREAD
    pthread_mutex_destroy(&running_lock);
    pthread_cond_destroy(&running_cond);
    #endif

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
