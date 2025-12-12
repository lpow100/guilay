#include "guilay.h"
#include <stdio.h>

int main() {
    printf("Bookmark");
    fflush(stdout);

    if (GuilayInit()) {
        // Handle GLFW initialization failure
        return -1; 
    }

    Window* window = CreateWindow((Vector2i){800, 600},"MyWindow");
    
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        GuilayExit();
        return -1; // <-- Crucial: Must return/exit here
    }

    LoadAssets(window);

    AddText(window,CreateText((Vector2){100,200},"Theo LOVES Oliva",0.80f,(Color){0,0,0}));

    while (!WindowShouldClose(window)) {

        FillWindow(window,(Color){20,20,20});

        UpdateWindow(window);
    }

    GuilayExit();
}