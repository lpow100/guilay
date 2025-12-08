#ifndef GUILAY_H
#define GUILAY_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_ELEMENTS 255

typedef enum {
    TEXT, BUTTON
} ElementType;
typedef struct Element Element;

// A window, chaging internal values without using the proper functions is not reccomended
typedef struct Window Window;
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

typedef struct Color{
    uint8_t red;
    uint8_t blue;
    uint8_t green;
} Color;

// initializes guilay
int GuilayInit();
// Cleanly exits guilay
void GuilayExit();

// Creates a window
// Items added to the window are layed out in order of how they are added by defualt.
Window* CreateWindow(Vector2i size, char* name);
// Renders the window
void PrepareWindow(Window* window);
// Fills a window with a color
void FillWindow(Window* window, Color fillColor);
// Updates the window
void UpdateWindow(Window* window);
// If the windows should close
bool WindowShouldClose(Window* window);

typedef struct Text Text;

// Creates a button that gets rendered
Text* CreateText(Vector2 size, char* text, float scale, Color color);
// Adds a button to the window
void AddText(Window* window, Text* button);


#endif