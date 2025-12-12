

#include "../src/shader.h"
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H  
// #include <GL/gl.h> // Remove: Already included via glad/glfw3, and often conflicts.
#include <stdio.h>
#include <stdlib.h> // Needed for exit/malloc/free if used in shader.c

#define MAT4_SIZE 16

typedef struct {
    int x, y;
}Vector2i;


// Assuming your custom shader functions are in this header
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    Vector2i   Size;       // Size of glyph
    Vector2i   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

struct Character characters[128];
unsigned int VAO, VBO;
Shader textShader;

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

void RenderText(Shader *s, char* text, float x, float y, float scale, float color[3])
{
    // activate corresponding render state	
    ShaderUse(s);
    glUniform3f(glGetUniformLocation(s->ID, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    for (int i = 0; i < strlen(text); i++)
    {
        struct Character ch = characters[text[i]];

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
}

int main(void)
{
    // --- Initialize GLFW ---
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Use modern OpenGL Core Profile (3.3 is common)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // --- Create window ---
    GLFWwindow* window = glfwCreateWindow(800, 600, "Shader Test", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // --- Load GLAD (must happen after context creation) ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // Set viewport dimensions
    glViewport(0, 0, 800, 600);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Could not init FreeType\n");
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../fonts/Roboto-Black.ttf", 0, &face))
    {
        printf("Failed to load font!\n"); 
        return -1;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);  
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Failed to load glyph: %d", c);
            continue;
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
        struct Character character = {
            texture, 
            {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            {face->glyph->bitmap_left, face->glyph->bitmap_top},
            face->glyph->advance.x
        };
        characters[c] = character;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

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
    
    // --- Initialize Shader ---
    Shader myShader;

    // Use actual shader filenames for testing (e.g., from the previous answer)
    // Ensure these files (basic.vert and basic.frag) exist next to your executable!
    if (!CreateShader(&myShader, "../src/fontShader.vert", "../src/fontShader.frag")) {
        fprintf(stderr, "Failed to initialize shader!\n");
        // Don't continue if shader fails to load
        return -1; 
    }

    float projection[MAT4_SIZE];

    create_ortho_matrix(projection,0.0f,800.0f,0.0f,600.0f,-1.0f,1.0f);

    GLint colorUniformLocation = glGetUniformLocation(myShader.ID, "projection");
    if (colorUniformLocation == -1) {
        fprintf(stderr, "Failed to find uniform!\n");
    }

    glUseProgram(myShader.ID);
    glUniformMatrix4fv(colorUniformLocation, 1, GL_FALSE, projection); // Set the uniform

    /*GLint textSamplerLocation = glGetUniformLocation(myShader.ID, "text");
    if (textSamplerLocation == -1) {
        fprintf(stderr, "Failed to find 'text' uniform!\n");
        // Handle error
    }
    glUniform1i(textSamplerLocation, 0);*/ // Binds the sampler 'text' to GL_TEXTURE0
    
    // --- Setup Buffers ---


    // --- Main loop ---
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen with a background color (e.g., a dark gray)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- Render Triangle ---
        
        // 1. Use the shader
        RenderText(&myShader, "Hello, World!", 20.0f, 550.0f, 1.0f, (float[3]){1.0f, 1.0f, 1.0f});

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- Cleanup ---
    // glDeleteBuffers(1, &VBO);
    // glDeleteVertexArrays(1, &VAO);
    // Shader_Destroy(&myShader); // If you implemented this in shader.c
    
    glfwTerminate();
    return 0;
}