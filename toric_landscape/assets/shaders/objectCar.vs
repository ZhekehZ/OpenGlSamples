#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_model;

out vec3 a_normal;
out vec3 a_position;
out vec2 a_tex;
out vec3 a_color;
out float a_mix;


void main() {
    a_normal = in_normal;
    a_position = vec3(u_model * vec4(in_position, 1.0));
    a_tex = in_tex.xy;
    a_color = in_color;
    a_mix = in_tex.z;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
