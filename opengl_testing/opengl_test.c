

#include "../src/shader.h"
#include <GLFW/glfw3.h>
// #include <GL/gl.h> // Remove: Already included via glad/glfw3, and often conflicts.
#include <stdio.h>
#include <stdlib.h> // Needed for exit/malloc/free if used in shader.c

// Assuming your custom shader functions are in this header

// --- VBO/VAO Globals (for the test triangle) ---
unsigned int VAO, VBO;

// --- Vertex Data (A simple green triangle in NDC) ---
float vertices[] = {
    // Coordinates (X, Y, Z) - in Normalized Device Coordinates (-1.0 to 1.0)
    -0.25f, -0.5f, 0.0f, // Bottom-left
     0.0f,  0.25f, 0.0f, // Bottom-right
    -0.675f,  0.0f, 0.0f  // Top-center
};

// --- Function to set up the VAO/VBO for the triangle ---
void setup_triangle_buffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0 from the basic.vert shader)
    // Size is 3 (vec3), Stride is 3*sizeof(float), offset is 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VBO, then VAO
    glsetup_triangle_buffersBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBinsetup_triangle_buffersdVertexArray(0);
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
    
    // --- Initialize Shader ---
    Shader myShader;

    // Use actual shader filenames for testing (e.g., from the previous answer)
    // Ensure these files (basic.vert and basic.frag) exist next to your executable!
    if (!CreateShader(&myShader, "../src/fontShader.vert", "../src/fontShader.frag")) {
        fprintf(stderr, "Failed to initialize shader!\n");
        // Don't continue if shader fails to load
        return -1; 
    }
    
    // --- Setup Buffers ---
    setup_triangle_buffers();


    // --- Main loop ---
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen with a background color (e.g., a dark gray)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- Render Triangle ---
        
        // 1. Use the shader
        ShaderUse(&myShader); 

        // NOTE: The uniform 'textColor' is not used by the basic.frag shader!
        // This line is just for demonstration, you'd remove it for the basic test.
        // Shader_SetVec3(&myShader, "textColor", 1.0f, 0.5f, 0.2f);

        // 2. Bind the VAO
        glBindVertexArray(VAO); 

        // 3. Draw the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3); 
        
        // 4. Unbind (optional, but good practice)
        glBindVertexArray(0);

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