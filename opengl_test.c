#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>

// Function to set up the 2D projection matrix
void setup_projection(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set up orthographic projection so we can use pixel coordinates easily (0 to 800 for X, 0 to 600 for Y)
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Function to draw the colored square
void draw_square() {
    // Define the colors for each corner using glColor3f(R, G, B)
    
    glBegin(GL_TRIANGLES);

    // Top-Left (Red Corner)
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex2f(100.0f, 300.0f); // X, Y coordinates

    // Top-Right (Yellow Corner, a mix of Red and Green)
    glColor3f(0.0f, 1.0f, 0.0f); 
    glVertex2f(300.0f, 300.0f);

    // Bottom-Right (Blue Corner - or whatever you prefer, maybe Magenta?)
    glColor3f(0.0f, 0.0f, 1.0f); 
    glVertex2f(200.0f, 125.0f);

    glEnd();
}

int main(void) {
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "OpenGL Colored Square", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set up our 2D projection
    setup_projection(800, 600);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen to black (default)

        draw_square();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
