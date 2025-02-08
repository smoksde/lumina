#version 430 core

out vec4 fragColor;

in vec2 v_tex_coord;

uniform sampler2D u_texture;
uniform vec4 u_color;

void main()
{
    vec4 sampledColor = texture(u_texture, v_tex_coord);
    fragColor = sampledColor.r * u_color;
}
