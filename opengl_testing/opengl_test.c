#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>

#include "../font.h"

void render_string(const char *text, float start_x, float start_y, float char_scale) {
    float x = start_x;

    glBegin(GL_TRIANGLES);

    for (const char *p = text; *p; p++) {

        uint8_t offset = *p - ' ';

        for (int row = 0; row < FONT_CHAR_HEIGHT; row++) {
            uint8_t row_data = font[row * 5 + offset];

            for (int col = 0; col < FONT_CHAR_WIDTH; col++) {

                if ((row_data >> (FONT_CHAR_WIDTH - 1 - col)) & 0b1 != 0) {

                    float px = x + col * char_scale;
                    float py = start_y - (FONT_CHAR_HEIGHT - 1 - row) * char_scale;

                    float x0 = px;
                    float y0 = py;
                    float x1 = px + char_scale;
                    float y1 = py + char_scale;

                    
                    printf("#");
                    
                    glColor3f(1.0f,1.0f,1.0f);

                    // Triangle 1
                    glVertex2f(x0, y0);
                    glVertex2f(x0, y1);
                    glVertex2f(x1, y1);

                    // Triangle 2
                    glVertex2f(x0, y0);
                    glVertex2f(x1, y1);
                    glVertex2f(x1, y0);
                } else {
                    printf(".");
                }
            }
            printf(" ");
        }

        // Add spacing between characters
        x += FONT_CHAR_WIDTH * char_scale + char_scale * 0.5f;
        printf("\n");
    }

    printf("\n\n");

    glEnd();
}

int main(void)
{
    // --- Initialize GLFW ---
    if (!glfwInit()) {
        return -1;
    }

    // --- Create window ---
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // --- Load GL ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to init GLAD\n");
        return -1;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1); // left, right, bottom, top, near, far

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // --- Main loop ---
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        render_string("test",100,100,2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
