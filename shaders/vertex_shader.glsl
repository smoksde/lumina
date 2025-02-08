#version 330 core

#define M_PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 i_pos;
layout (location = 2) in float i_rot;

uniform mat4 view;
uniform mat4 projection;

void main() {
    mat4 model = mat4(1.0); // Initialize as identity matrix
    model[3] = vec4(i_pos, 1.0);

    // Rotation around Z-axis
    // float angle = radians(i_rot);
    float angle = -i_rot * M_PI / 180.0;
    float c = cos(angle);
    float s = sin(angle);
    mat4 rotation_z = mat4(c, -s, 0.0, 0.0,
                          s, c, 0.0, 0.0,
                          0.0, 0.0, 1.0, 0.0,
                          0.0, 0.0, 0.0, 1.0);
    
    model = model * rotation_z;

    // Final transformation
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}
