#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 ambientColor; // e.g., a constant dark light

void main()
{
    // The final color is just the object color tinted by ambient light.
    FragColor = vec4(objectColor * ambientColor, 1.0);
}