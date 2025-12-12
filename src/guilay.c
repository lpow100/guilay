#include <ft2build.h>
#include FT_FREETYPE_H  
#include <glad/glad.h>       // MUST be first OpenGL include
#include <GLFW/glfw3.h>      // MUST come after glad

#include "guilay.h"
#include "common/font.h"
#include "common/shader.h"
#include "common/elements.h"

#include <stdlib.h>

#define CHARACTER_LOAD_COUNT 128
#define MAT4_SIZE 16

// ----------- Structures -----------

struct Window {
    GLFWwindow* openglWindow;
    Vector2i size;
    Element** elements;
    size_t elementCount;
};

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    Vector2i   Size;       // Size of glyph
    Vector2i   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

struct Character characters[CHARACTER_LOAD_COUNT];
unsigned int VAO, VBO;
Shader textShader;

// ----------- Init / Exit -----------

int GuilayInit() {

    if(glfwInit() == GLFW_FALSE) return 1;

    // Force compatibility mode so glBegin(), glMatrixMode(), etc work
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return 0;
}

void GuilayExit() {
    glfwTerminate();
}

void create_ortho_matrix(float *M, 
                         float left, float right, 
                         float bottom, float top, 
                         float near, float far) 
{
    // The matrix is initialized to zero
    for (int i = 0; i < MAT4_SIZE; i++) {
        M[i] = 0.0f;
    }

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(far + near) / (far - near);

    // Column 0 (M[0], M[1], M[2], M[3])
    M[0] = 2.0f / (right - left);
    // M[1] = 0.0f
    // M[2] = 0.0f
    // M[3] = 0.0f

    // Column 1 (M[4], M[5], M[6], M[7])
    // M[4] = 0.0f
    M[5] = 2.0f / (top - bottom);
    // M[6] = 0.0f
    // M[7] = 0.0f

    // Column 2 (M[8], M[9], M[10], M[11])
    // M[8] = 0.0f
    // M[9] = 0.0f
    M[10] = -2.0f / (far - near);
    // M[11] = 0.0f

    // Column 3 (M[12], M[13], M[14], M[15]) - Translation column
    M[12] = tx;
    M[13] = ty;
    M[14] = tz;
    M[15] = 1.0f;
}

int LoadAssets(Window* window) {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    FT_Library ft;
    int errorCode = FT_Init_FreeType(&ft);
    printf("FT first call said: %d\n",errorCode);
    if (errorCode) return 1;
    FT_Face face;
    errorCode = FT_New_Face(ft, "fonts/Roboto-Black.ttf", 0, &face);
    printf("FT Second call said: %d\n",errorCode);
    if (errorCode) return 1;

    FT_Set_Pixel_Sizes(face, 0, 48); 

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    printf("Bookmark\n");
    fflush(stdout);
  
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

    printf("Bookmark\n");
    fflush(stdout);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
    
    CreateShader(&textShader, "src/shaders/fontShader.vert", "src/shaders/fontShader.frag");

    float projection[MAT4_SIZE];

    create_ortho_matrix(projection,0.0f,(float)window->size.x,0.0f,(float)window->size.y,-1.0f,1.0f);

    GLint colorUniformLocation = glGetUniformLocation(textShader.ID, "projection");
    if (colorUniformLocation == -1) {
        fprintf(stderr, "Failed to find uniform!\n");
    }

    glUseProgram(textShader.ID);
    glUniformMatrix4fv(colorUniformLocation, 1, GL_FALSE, projection);

    return 0;
}

// ----------- Window creation -----------

Window *CreateWindow(Vector2i size, char* name) {
    Window *window = (Window*)malloc(sizeof(Window));
    window->size = size;

    window->openglWindow = glfwCreateWindow(size.x, size.y, name, NULL, NULL);
    window->elementCount = 0;
    window->elements = NULL;

    glfwMakeContextCurrent(window->openglWindow);

    return window;
}


// ----------- Projection setup -----------


// ----------- Clear -----------

void FillWindow(Window* window, Color fillColor) {
    glClearColor(fillColor.red / 255.0f,
                 fillColor.green / 255.0f,
                 fillColor.blue / 255.0f,
                 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
}


// ----------- Text Rendering (compatibility mode) -----------

void RenderText(Shader *s, char* text, float x, float y, float scale, Color color)
{
    // activate corresponding render state	
    ShaderUse(s);
    glUniform3f(glGetUniformLocation(s->ID, "textColor"), color.red/255, color.green/255, color.blue/255);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    for (int i = 0; i < strlen(text); i++)
    {
        Character ch = characters[text[i]];

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
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
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
    glUseProgram(0)
}


// ----------- Update Window -----------

void UpdateWindow(Window* window) {
    for (size_t i = 0; i < window->elementCount; i++) {
        Element* element = window->elements[i];

        if (element->type == TEXT) {
            Text* t = element->data;
            RenderText(&textShader, t->text, 20, window->size.y - 40, t->scale, (Color){255,255,255});
        }
    }

    glfwSwapBuffers(window->openglWindow);
    glfwPollEvents();
}

bool WindowShouldClose(Window* window) {
    return glfwWindowShouldClose(window->openglWindow);
}

void AddElement(Window* window, Element* element) {
    ResizeElementsArray(window->elements, &window->elementCount, window->elementCount + 1);
    window->elements[window->elementCount - 1] = element;
}