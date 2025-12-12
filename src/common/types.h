#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

typedef struct Rect
{
    Vector2 position;
    Vector2 size;
} Rect;

typedef struct Color{
    uint8_t red;
    uint8_t blue;
    uint8_t green;
    uint8_t alpha;
} Color;

#endif