#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_mvp1;

out vec3 a_shadow;

out vec3 a_normal;
out vec3 a_position;
out vec3 a_tex;

void main() {
    a_shadow = vec3(u_mvp1 * vec4(in_position, 1.0));
    a_normal = in_normal;
    a_position = in_position;
    a_tex = in_tex;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
