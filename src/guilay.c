#include <ft2build.h>
#include FT_FREETYPE_H  
#include <glad/glad.h>       // MUST be first OpenGL include
#include <GLFW/glfw3.h>      // MUST come after glad

#include "guilay.h"
#include "font.h"
#include "shader.h"

#include <stdlib.h>

#define CHARACTER_LOAD_COUNT 128

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

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    Vector2i   Size;       // Size of glyph
    Vector2i   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

struct Character characters[CHARACTER_LOAD_COUNT];


// ----------- Init / Exit -----------

int GuilayInit() {
    if(glfwInit() == GLFW_FALSE) return 1;

    // Force compatibility mode so glBegin(), glMatrixMode(), etc work
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) return 1;
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) return 1;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Failed to load glyph: %d\n", c);
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            (Vector2i) {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            (Vector2i) {face->glyph->bitmap_left, face->glyph->bitmap_top},
            face->glyph->advance.x
        };
        characters[c] = character;
    }   

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      

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

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    s.Use();
    glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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
