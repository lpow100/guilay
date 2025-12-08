#include "guilay.h"
#include "font.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdlib.h>

struct Element {
    ElementType type;
    void* data;
};

// A window, chaging internal values without using the proper functions is not reccomended
struct Window {
    GLFWwindow* openglWindow;
    Vector2i size;
    Element** elements;
    size_t elementCount;
};

struct Text {
    Vector2 size;
    float scale;
    char* text;
    Color color;
};

int GuilayInit() { return glfwInit(); }
void GuilayExit() { glfwTerminate(); }

Window *CreateWindow(Vector2i size, char* name) {
    Window *window = (Window*)malloc(sizeof(Window));
    window->size = size;
    window->openglWindow = glfwCreateWindow(size.x, size.y, "OpenGL Colored Square", NULL, NULL);
    window->elementCount = 0;
    return window;
}

void setup_projection(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set up orthographic projection so we can use pixel coordinates easily (0 to 800 for X, 0 to 600 for Y)
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void PrepareWindow(Window* window) {
    glfwMakeContextCurrent(window->openglWindow);
    setup_projection(window->size.x,window->size.y);
}

void FillWindow(Window* window, Color fillColor) {
    glClearColor(fillColor.red / 255, fillColor.green / 255, fillColor.blue / 255, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

// Renders a string starting at screen position (x, y)
void render_string(const char *text, float start_x, float start_y, float char_scale) {
    float current_x = start_x;
    
    // 1. Prepare Vector2 Buffer (for dynamic data)
    // NOTE: In a real app, you'd use a dynamic VBO/VAO setup.
    // Here we'll just define an array and draw it.
    
    // Max size: Max string length * CHAR_WIDTH * CHAR_HEIGHT * 6 vertices
    Vector2 vertices[512 * FONT_CHAR_WIDTH * FONT_CHAR_HEIGHT * 6]; 
    int vert_count = 0;

    for (const char *p = text; *p != '\0'; p++) {
        uint8_t offset = ' ' - *p;
        
        // Iterate through rows (Y-axis) of the 5x5 bitmap
        for (int row = 0; row < FONT_CHAR_HEIGHT; row++) {
            // Get the 8-bit row data.
            uint8_t row_data = font[row*5+offset];
            
            // Iterate through columns (X-axis) of the 5x5 bitmap
            for (int col = 0; col < FONT_CHAR_WIDTH; col++) {
                // Check if the pixel is 'on' (bit is 1).
                // Bits are read right-to-left: bit 0 is the rightmost pixel.
                // To read left-to-right, we check the bit at (5 - 1 - col).
                if ((row_data >> (FONT_CHAR_WIDTH - 1 - col)) & 0x1) {
                    
                    // --- Generate Quad Vertices ---
                    
                    // Pixel position in screen space, scaled.
                    float px = current_x + (float)col * char_scale;
                    // Note: Flip Y if needed. If row 0 is the top, (HEIGHT - 1 - row) is used.
                    float py = start_y - (float)(FONT_CHAR_HEIGHT - 1 - row) * char_scale;
                    
                    // Corner coordinates (for a 1-unit square, then scaled)
                    float x0 = px;
                    float y0 = py;
                    float x1 = px + char_scale;
                    float y1 = py + char_scale;

                    // Vertices (2 triangles/6 vertices)
                    // Triangle 1 (Bottom-Left, Top-Left, Top-Right)
                    vertices[vert_count++] = (Vector2){x0, y0};
                    vertices[vert_count++] = (Vector2){x0, y1};
                    vertices[vert_count++] = (Vector2){x1, y1};

                    // Triangle 2 (Bottom-Left, Top-Right, Bottom-Right)
                    vertices[vert_count++] = (Vector2){x0, y0};
                    vertices[vert_count++] = (Vector2){x1, y1};
                    vertices[vert_count++] = (Vector2){x1, y0};
                }
            }
        }
        
        // Advance current_x by the width of the character plus some spacing.
        current_x += FONT_CHAR_WIDTH * char_scale + char_scale * 0.5f; 
    }
    
    // 2. Draw the generated vertices
    // Assuming you have bound your VAO and shader program already
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(Vector2), vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vert_count);
}

void UpdateWindow(Window* window) {
    for (int i = 0; i < window->elementCount; i++) {
        Element* element = window->elements[i];
        if (element->type == TEXT) {
            Text* text = element->data;
            render_string(text->text,1000,100,text->scale);
        }
    }
    glfwSwapBuffers(window->openglWindow);
    glfwPollEvents();
}

bool WindowShouldClose(Window* window) {
    return glfwWindowShouldClose(window->openglWindow);
}

// Helper function to resize the array of element pointers
void ResizeElementsArray(Window* window, size_t newCount) {
    // Reallocate memory for the new size
    // Note: realloc returns NULL on failure, handle this in a real app
    window->elements = (Element**)realloc(window->elements, newCount * sizeof(Element*));
    if (window->elements == NULL && newCount > 0) {
        // Handle reallocation failure (e.g., print error and exit)
    }
    window->elementCount = newCount;
}

// Helper to create a new Element structure
Element* CreateElement(ElementType type, void* data) {
    // Allocate memory for the new Element struct
    Element* newElement = (Element*)malloc(sizeof(Element));
    if (newElement == NULL) {
    }

    newElement->type = type;
    newElement->data = data;

    return newElement;
}

Text* CreateText(Vector2 size, char* text, float scale, Color color) {
    Text* textElement = (Text*) malloc(sizeof(Text));
    textElement->size = size;
    textElement->text = text;
    textElement->color = color;
    textElement->scale = scale;
    return textElement;
}

void AddText(Window* window, Text* button) {
    ResizeElementsArray(window,window->elementCount+1);
    window->elements[window->elementCount+1] = CreateElement(TEXT, button);
}
