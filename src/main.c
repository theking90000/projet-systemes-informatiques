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
    Led led;
    Color c = {132, 3, 252, 0};
    int i;

    char input[255] = {0};


    FILE* fdi;
    
    for (i = 0; i < argc; i++) {
	if (strcmp(argv[i], "--input") == 0) {
	    if (i+1 >= argc) {
	        printf("Mauvais usage: --input attend un argument\n");
                exit(1);
	    }
	    strncpy(input, argv[i+1], 255);
	}
    }

    printf("Debug: input=%s\n", input);

    if (strlen(input) == 0) {
        fdi = stdin;
    } else {
    	fdi = fopen(input, "r");
	if (fdi == NULL) {
	    printf("Erreur\n");
	    exit(1);
	}
    }

    init_led(&led, RED_PIN, GREEN_PIN, BLUE_PIN, POWER_PIN);

    set_color(&led, RED);
    delay(1000);

    set_color(&led, GREEN);
    delay(1000);

    for (uint8_t i = 255; i > 0; i -=5) {
	c.alpha = i;
        set_color(&led, c);
        delay(50);
    }

    turn_off(&led);

    return 0;
}
