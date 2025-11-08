#include "led.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdbool.h>

static int led_initialized = false;

int initialise() {
    if (led_initialized) {
        printf("Warn: LED already initialized.\n");
        return 0;
    }
    wiringPiSetupGpioDevice(WPI_PIN_BCM);
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(POWER_PIN, OUTPUT);

    softPwmCreate(RED_PIN, 0, 255);
    softPwmCreate(GREEN_PIN, 0, 255);
    softPwmCreate(BLUE_PIN, 0, 255);
    softPwmCreate(POWER_PIN, 0, 255);

    led_initialized = true;
    return 0;
}

void set_color(Color color) {
    softPwmWrite(RED_PIN, 255 - color.red);
    softPwmWrite(GREEN_PIN, 255 - color.green);
    softPwmWrite(BLUE_PIN, 255 - color.blue);
    softPwmWrite(POWER_PIN, color.alpha);
}

void turn_off() {
    if (!led_initialized) {
        printf("Warn: LED not initialized.\n");
        return;
    }
    softPwmWrite(RED_PIN, 255);
    softPwmWrite(GREEN_PIN, 255);
    softPwmWrite(BLUE_PIN, 255);
    softPwmWrite(POWER_PIN, 0);
    softPwmStop(RED_PIN);
    softPwmStop(GREEN_PIN);
    softPwmStop(BLUE_PIN);
    softPwmStop(POWER_PIN);
    led_initialized = false;
}