#include "led.h"

#include <stdio.h>
#include <wiringPi.h>

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26

int main() {
    Led led;
    Color c = {132, 3, 252, 0};

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
