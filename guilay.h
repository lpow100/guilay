#ifndef GUILAY_H
#define GUILAY_H

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#define MAX_ELEMENTS 255

typedef enum {
    BUTTON,
    LABEL
} ElementType;

typedef struct Element {
    ElementType type;
    void* data; 
    float x, y, width, height;
} Element;

// A window, chaging internal values without using the proper functions is not reccomended
typedef struct {
    GLFWwindow* openglWindow;
    Element** elements;
    size_t elementCount;
} Window;

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    uint8_t red;
    uint8_t blue;
    uint8_t green;
} Color;

// initializes guilay
int GuilayInit();
// Cleanly exits guilay
void GuilayExit();

// Creates a window
Window CreateWindow(Vector2 size, char* name);

typedef struct {
    Vector2 size;
    char* text;
    Color color;
} Button;

// Creates a button that gets rendered
Button CreateButton(Vector2 size, char* text, Color color);

void AddButton(Window* window, Button* button);


#endif