#include "led.h"

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <string.h>

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26

int main(int argc, char *argv[]) {
    /* Param√tres du programme */
    char     input[255] = {0}; /* --input <path> (optionel)  */
    char     output[255] = {0}; /* --output <path> (optionel) */
    int      only_longest = 0; /* --only-longest (optionnel) */
    int      debug = 0; /* --debug <level> (optionnel) */

    /* Variables utilis√es par le programme */
    Led      led;
    int      i;
    FILE*    in;
    FILE*    out;

    /* D√tecter les param√®trespass√s en argument du programme */
    for (i = 0; i < argc; i++) {
	if (strcmp(argv[i], "--input") == 0) {
	    if (i+1 >= argc) {
	        printf("Mauvais usage: --input attend un argument\n");
                exit(1);
	    }
	    
	    strncpy(input, argv[i+1], 255);
	    i++;
	}

	if(strcmp(argv[i], "--output") == 0) {
	    if(i+1 >= argc) {
	        printf("Mauvais usage: --output attend un argument\n");
	        exit(1);
	    }
	    
	    strncpy(output, argv[i+1], 255);
	    i++;
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

	    if (sscanf(argv[i+1], "%d", &debug) == 0) {
	    	printf("Mauvais usage: --debug attend un argument entier\n");
		exit(1);
	    }
            i++;
        }
    }

    printf("Debug Params\n");
    printf(" - input='%s'\n", input);
    printf(" - output='%s'\n", output);
    printf(" - only_longest=%b\n", only_longest);
    printf(" - debug=%d\n", debug);

    if (strlen(input) == 0) {
        //fdi = stdin;
    } else {
    	/*fdi = fopen(input, "r");
	if (fdi == NULL) {
	    printf("Erreur\n");
	    exit(1);
	}*/
    }

    init_led(&led, RED_PIN, GREEN_PIN, BLUE_PIN, POWER_PIN);

    set_color(&led, RED);
    delay(1000);

    set_color(&led, GREEN);
    delay(1000);

    for (uint8_t i = 255; i > 0; i -=5) {
	//c.alpha = i;
        //set_color(&led, c);
        delay(50);
    }

    turn_off(&led);

    return 0;
}
