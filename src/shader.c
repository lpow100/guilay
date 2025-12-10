#include "shader.h"

// --- Helper function to read file contents into a string ---
char* readFile(const char* path) {
    FILE *fp;
    long len;
    char *buf;

    fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_FOUND: %s\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = (char*)malloc(len + 1);
    if (!buf) {
        fclose(fp);
        fprintf(stderr, "ERROR::SHADER::MEMORY_ALLOCATION_FAILED\n");
        return NULL;
    }

    fread(buf, 1, len, fp);
    buf[len] = '\0'; // Null-terminate the string
    fclose(fp);
    return buf;
}

// --- Error Checking Utility ---
void checkCompileErrors(unsigned int shader, const char* type) {
    int success;
    char infoLog[1024];
    
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
}

// --- Initialization Function (replaces constructor) ---
int CreateShader(Shader* s, const char* vertexPath, const char* fragmentPath) {
    // 1. Retrieve the vertex/fragment source code from files
    char* vShaderCode = readFile(vertexPath);
    char* fShaderCode = readFile(fragmentPath);

    if (!vShaderCode || !fShaderCode) {
        // Free memory if one succeeded but the other failed
        free(vShaderCode);
        free(fShaderCode);
        return 0; // Failure
    }

    // 2. Compile shaders
    unsigned int vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const GLchar* const*)&vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const GLchar* const*)&fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Free the dynamically allocated strings after they are passed to OpenGL
    free(vShaderCode);
    free(fShaderCode);

    // 3. Shader Program Link
    s->ID = glCreateProgram();
    glAttachShader(s->ID, vertex);
    glAttachShader(s->ID, fragment);
    glLinkProgram(s->ID);
    checkCompileErrors(s->ID, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return 1; // Success
}

// --- Use Function (replaces Use() method) ---
void ShaderUse(const Shader* s) {
    glUseProgram(s->ID);
}

// --- Utility Uniform Functions ---

void ShaderSetBool(const Shader* s, const char* name, int value) {
    glUniform1i(glGetUniformLocation(s->ID, name), value); 
}

void ShaderSetInt(const Shader* s, const char* name, int value) {
    glUniform1i(glGetUniformLocation(s->ID, name), value); 
}

void ShaderSetFloat(const Shader* s, const char* name, float value) {
    glUniform1f(glGetUniformLocation(s->ID, name), value); 
}