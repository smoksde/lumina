#version 430 core

in vec2 a_position;
in vec2 a_tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_modelViewProj;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 0.5, 1.0);

    v_tex_coord = a_tex_coord;
}