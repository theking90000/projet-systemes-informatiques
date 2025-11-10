#ifndef LED_GUARD
#define LED_GUARD

#include <stdint.h>

#define RED ((Color){255, 0, 0, 255})
#define GREEN ((Color){0, 255, 0, 255})
#define BLUE ((Color){0, 0, 255, 255})
#define WHITE ((Color){255, 255, 255, 255})

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26

typedef struct   Color {
        
        uint8_t  red;
        uint8_t  green;
        uint8_t  blue;
        uint8_t  alpha;

}                Color;

typedef struct   Led {
        
        uint8_t  red_pin;
        uint8_t  blue_pin;
        uint8_t  green_pin;
        uint8_t  power_pin;

}                Led;


int     init_led(Led     *led, 
                 uint8_t red_pin, 
                 uint8_t green_pin, 
                 uint8_t blue_pin, 
                 uint8_t power_pin);

void 	set_color(Led   *led, 
                  Color  color);

void    turn_off(Led *led);

#endif
