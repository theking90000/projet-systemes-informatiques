#include "led.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

int create_pwm(Led* led) {
    if (led->soft_pwm != 0) {
        pinMode(led->red_pin, OUTPUT);
        pinMode(led->green_pin, OUTPUT);
        pinMode(led->blue_pin, OUTPUT);
        pinMode(led->power_pin, OUTPUT);

        /* TODO: gerer les erreurs (!=0) ici */
        softPwmCreate(led->red_pin, 0, 255);
        softPwmCreate(led->green_pin, 0, 255);
        softPwmCreate(led->blue_pin, 0, 255);
        softPwmCreate(led->power_pin, 0, 255);
    } else {
        pinMode(led->red_pin, PWM_OUTPUT);
        pinMode(led->green_pin, PWM_OUTPUT);
        pinMode(led->blue_pin, PWM_OUTPUT);
        pinMode(led->power_pin, PWM_OUTPUT);
    }

    return 0;
}

void destroy_pwm(Led* led) {
    if (led->soft_pwm != 0) {
        softPwmStop(led->red_pin);
        softPwmStop(led->green_pin);
        softPwmStop(led->blue_pin);
        softPwmStop(led->power_pin);
    }
}

int init_led(Led*    led, 
             uint8_t red_pin, 
             uint8_t green_pin, 
             uint8_t blue_pin, 
             uint8_t power_pin,
             uint8_t soft_pwm) {
    if(wiringPiSetupGpioDevice(WPI_PIN_BCM) != 0) {
        return 1;
    }

    led->red_pin = red_pin;
    led->green_pin = green_pin;
    led->blue_pin = blue_pin;
    led->power_pin = power_pin;
    
    if(create_pwm(led) != 0) {
        return 1;
    }

    return 0;
}

void set_color(Led *led, Color color) {
    if (led->soft_pwm != 0) {
        softPwmWrite(led->red_pin, 255 - color.red);
        softPwmWrite(led->green_pin, 255 - color.green);
        softPwmWrite(led->blue_pin, 255 - color.blue);
        softPwmWrite(led->power_pin, color.alpha);
    } else {
        pwmWrite(led->red_pin, 255 - color.red);
        pwmWrite(led->green_pin, 255 - color.green);
        pwmWrite(led->blue_pin, 255 - color.blue);
        pwmWrite(led->power_pin, color.alpha);
    }
}

void turn_off(Led *led) {
    destroy_pwm(led);

    pinMode(led->red_pin, PM_OFF);
    pinMode(led->green_pin, PM_OFF);
    pinMode(led->blue_pin, PM_OFF);
    pinMode(led->power_pin, PM_OFF);
}
