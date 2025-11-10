#include "led.h"
#include "color.h"
#include <stdio.h>
#include <wiringPi.h>

int main() {
    if (initialise() != 0) return 1;

    set_color(RED);
    delay(1000);

    set_color(GREEN);
    delay(1000);

    for (uint8_t i = 255; i > 0; i -=5) {
        set_color(color(132, 3, 252, i));
        delay(50);
    }

    turn_off();

    return 0;
}