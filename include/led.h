#pragma once
#include <stdint.h>
#include "color.h"

#define RED_PIN 22
#define GREEN_PIN 27
#define BLUE_PIN 17
#define POWER_PIN 26

int initialise();
void set_color(Color color);
void turn_off();