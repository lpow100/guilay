#include <ft2build.h>
#include FT_FREETYPE_H  
#include <glad/glad.h>       // MUST be first OpenGL include
#include <GLFW/glfw3.h>      // MUST come after glad

#include "guilay.h"
#include "font.h"

#include <stdlib.h>


// ----------- Structures -----------

struct Element {
    ElementType type;
    void* data;
};

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


// ----------- Init / Exit -----------

int GuilayInit() {
    if(glfwInit() == GLFW_FALSE) return 1;

    // Force compatibility mode so glBegin(), glMatrixMode(), etc work
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) return 1;
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) return 1;

    return 0;
}

void GuilayExit() {
    glfwTerminate();
}


// ----------- Window creation -----------

Window *CreateWindow(Vector2i size, char* name) {
    Window *window = (Window*)malloc(sizeof(Window));
    window->size = size;

    window->openglWindow = glfwCreateWindow(size.x, size.y, name, NULL, NULL);
    window->elementCount = 0;
    window->elements = NULL;

    return window;
}


// ----------- Projection setup -----------

void setup_projection(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void PrepareWindow(Window* window) {
    glfwMakeContextCurrent(window->openglWindow);
    gladLoadGL(); // Load GL function pointers through glad
    setup_projection(window->size.x, window->size.y);
}


// ----------- Clear -----------

void FillWindow(Window* window, Color fillColor) {
    glClearColor(fillColor.red / 255.0f,
                 fillColor.green / 255.0f,
                 fillColor.blue / 255.0f,
                 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
}


// ----------- Text Rendering (compatibility mode) -----------

void render_string(const char *text, float start_x, float start_y, float char_scale) {
    float x = start_x;

    glBegin(GL_TRIANGLES);

    for (const char *p = text; *p; p++) {

        uint8_t offset = ' ' - *p;

        for (int row = 0; row < FONT_CHAR_HEIGHT; row++) {
            uint8_t row_data = font[row * 5 + offset];

            for (int col = 0; col < FONT_CHAR_WIDTH; col++) {

                if ((row_data >> (FONT_CHAR_WIDTH - 1 - col)) & 1) {

                    float px = x + col * char_scale;
                    float py = start_y - (FONT_CHAR_HEIGHT - 1 - row) * char_scale;

                    float x0 = px;
                    float y0 = py;
                    float x1 = px + char_scale;
                    float y1 = py + char_scale;

                    // Triangle 1
                    glVertex2f(x0, y0);
                    glVertex2f(x0, y1);
                    glVertex2f(x1, y1);

                    // Triangle 2
                    glVertex2f(x0, y0);
                    glVertex2f(x1, y1);
                    glVertex2f(x1, y0);
                }
            }
        }

        // Add spacing between characters
        x += FONT_CHAR_WIDTH * char_scale + char_scale * 0.5f;
    }

    glEnd();
}


// ----------- Update Window -----------

void UpdateWindow(Window* window) {
    for (size_t i = 0; i < window->elementCount; i++) {
        Element* element = window->elements[i];

        if (element->type == TEXT) {
            Text* t = element->data;
            render_string(t->text, 20, window->size.y - 40, t->scale);
        }
    }

    glfwSwapBuffers(window->openglWindow);
    glfwPollEvents();
}

bool WindowShouldClose(Window* window) {
    return glfwWindowShouldClose(window->openglWindow);
}


// ----------- Element Management -----------

void ResizeElementsArray(Window* window, size_t newCount) {
    window->elements = (Element**)realloc(window->elements, newCount * sizeof(Element*));
    window->elementCount = newCount;
}

Element* CreateElement(ElementType type, void* data) {
    Element* e = (Element*)malloc(sizeof(Element));
    e->type = type;
    e->data = data;
    return e;
}

Text* CreateText(Vector2 size, char* text, float scale, Color color) {
    Text* t = (Text*)malloc(sizeof(Text));
    t->size = size;
    t->text = text;
    t->color = color;
    t->scale = scale;
    return t;
}

void AddText(Window* window, Text* txt) {
    ResizeElementsArray(window, window->elementCount + 1);
    window->elements[window->elementCount - 1] = CreateElement(TEXT, txt);
}
