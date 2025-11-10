#pragma once
#include <stdint.h>

typedef struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} Color;

#define RED ((Color){255, 0, 0, 255})
#define GREEN ((Color){0, 255, 0, 255})
#define BLUE ((Color){0, 0, 255, 255})
#define WHITE ((Color){255, 255, 255, 255})

Color color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);