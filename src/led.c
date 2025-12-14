#include "led.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

int init_led(Led*    led, 
             uint8_t red_pin, 
             uint8_t green_pin, 
             uint8_t blue_pin, 
             uint8_t power_pin) {
    if(wiringPiSetupGpioDevice(WPI_PIN_BCM) != 0) {
        return 1;
    }

    led->red_pin = red_pin;
    led->green_pin = green_pin;
    led->blue_pin = blue_pin;
    led->power_pin = power_pin;

    pinMode(led->red_pin, OUTPUT);
    pinMode(led->green_pin, OUTPUT);
    pinMode(led->blue_pin, OUTPUT);
    pinMode(led->power_pin, OUTPUT);

    return 0;
}

void set_color(Led *led, Color color) {
    // HIGH= pas de couleur, LOW=couleur
    digitalWrite(led->red_pin, color.red == 0 ? HIGH : LOW);
    digitalWrite(led->green_pin, color.green == 0 ? HIGH : LOW);
    digitalWrite(led->blue_pin, color.blue == 0 ? HIGH : LOW);
    digitalWrite(led->power_pin, color.alpha == 0 ? LOW : HIGH);
}

void turn_off(Led *led) {
    pinMode(led->red_pin, PM_OFF);
    pinMode(led->green_pin, PM_OFF);
    pinMode(led->blue_pin, PM_OFF);
    pinMode(led->power_pin, PM_OFF);
}
