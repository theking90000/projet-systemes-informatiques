#include "led.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

int init_led(Led *led, uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin, uint8_t power_pin) {
    wiringPiSetupGpioDevice(WPI_PIN_BCM);

    pinMode(red_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(power_pin, OUTPUT);

    softPwmCreate(red_pin, 0, 255);
    softPwmCreate(green_pin, 0, 255);
    softPwmCreate(blue_pin, 0, 255);
    softPwmCreate(power_pin, 0, 255);

    led->red_pin = red_pin;
    led->green_pin = green_pin;
    led->blue_pin = blue_pin;
    led->power_pin = power_pin;

    return 0;
}

void set_color(Led *led, Color color) {
    softPwmWrite(led->red_pin, 255 - color.red);
    softPwmWrite(led->green_pin, 255 - color.green);
    softPwmWrite(led->blue_pin, 255 - color.blue);
    softPwmWrite(led->power_pin, color.alpha);
}

void turn_off(Led *led) {
    softPwmWrite(led->red_pin, 255);
    softPwmWrite(led->green_pin, 255);
    softPwmWrite(led->blue_pin, 255);
    softPwmWrite(led->power_pin, 0);

    softPwmStop(led->red_pin);
    softPwmStop(led->green_pin);
    softPwmStop(led->blue_pin);
    softPwmStop(led->power_pin);
}
