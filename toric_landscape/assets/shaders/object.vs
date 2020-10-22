#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_mv_normal;
uniform mat4 u_mvp_light_near;

out vec3 a_position;
out vec3 a_normal;
out vec3 a_tex;
out vec3 a_color;
out vec3 a_shadow;

void main() {
    vec4 shadow_coord = u_mvp_light_near * vec4(in_position, 1.0);
    a_shadow = shadow_coord.xyz / shadow_coord.w / 2 + 0.5;

    a_position = vec3(u_model * vec4(in_position, 1.0));
    a_normal = vec3(u_mv_normal * vec4(in_normal, 1));
    a_tex = in_tex;
    a_color = in_color;

    gl_Position = u_mvp * vec4(in_position, 1.0);
}
