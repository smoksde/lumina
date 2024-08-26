#version 330 core

#define M_PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 i_pos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    mat4 model = mat4(1.0); // Initialize as identity matrix
    model[3] = vec4(i_pos, 1.0);

    // Final transformation
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}
