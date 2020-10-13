#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tex;

uniform mat4 u_mvp;

out vec3 a_normal;
out vec3 a_position;
out vec3 a_tex;

void main() {

    a_normal = in_normal;
    a_position = in_position;
    a_tex = in_tex;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
