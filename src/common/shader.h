#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GL/gl.h> // Or use GL/gl.h, depending on your setup

typedef struct {
    unsigned int ID;
} Shader;

int CreateShader(Shader *s, const char* vertexPath, const char* fragmentPath);

void ShaderUse(const Shader* s);

// --- Utility Uniform Functions ---

void ShaderSetBool(const Shader* s, const char* name, int value);
void ShaderSetInt(const Shader* s, const char* name, int value);
void ShaderSetFloat(const Shader* s, const char* name, float value);

#endif