#include "guilay.h"

int main() {
    if (!GuilayInit()) {
        // Handle GLFW initialization failure
        return -1; 
    }

    Window* window = CreateWindow((Vector2i){800, 600},"MyWindow"); 

    AddText(window,CreateText((Vector2){100,200},"Hello",2.0f,(Color){0,0,0}));

    while (!WindowShouldClose(window)) {
        PrepareWindow(window);

        FillWindow(window,(Color){255,255,0});

        UpdateWindow(window);
    }

    GuilayExit();
}