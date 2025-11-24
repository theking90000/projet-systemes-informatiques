#ifndef LED_GUARD
#define LED_GUARD

#include <stdint.h>

#define RED ((Color){255, 0, 0, 255})
#define GREEN ((Color){0, 255, 0, 255})
#define BLUE ((Color){0, 0, 255, 255})
#define WHITE ((Color){255, 255, 255, 255})

#define NO_PWM 0
#define SOFT_PWM 1
// HARD_PWM est jamais utilisé
#define HARD_PWM 2

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
 
        /* Utilisation du PWM (Pulse Width Modulation) 
         * 0 = Pas de PWM (bit low ou high, couleurs ON ou OFF)
         * 1 = PWM Software (utilisation de threads de controles via wiringpi)
         * 2 = PWM Hardware (utilisation du PWM hardware du raspberry : seulement certains pin */
        uint8_t  pwm_type;

}                Led;


int     init_led(Led     *led, 
                 uint8_t red_pin, 
                 uint8_t green_pin, 
                 uint8_t blue_pin, 
                 uint8_t power_pin,
                 uint8_t pwm_type);

void 	set_color(Led   *led, 
                  Color  color);

void    turn_off(Led *led);

#endif
