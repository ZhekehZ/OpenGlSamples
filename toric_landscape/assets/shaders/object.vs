#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec3 in_color_emission;
layout (location = 4) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_mv_normal;

out vec3 a_position;
out vec3 a_position_raw;
out vec3 a_normal;
out vec3 a_tex;
out vec3 a_color;
out vec3 a_color_emission;

void main() {
    a_position = vec3(u_model * vec4(in_position, 1.0));
    a_normal = vec3(u_mv_normal * vec4(in_normal, 1));
    a_tex = in_tex;
    a_position_raw = in_position;
    a_color = in_color;
    a_color_emission = in_color_emission;

    gl_Position = u_mvp * vec4(in_position, 1.0);
}
