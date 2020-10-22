#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_mvp1;
uniform mat4 u_model;
uniform mat4 u_mv_normal;
uniform mat4 u_mvp_big;

out vec3 a_normal;
out vec3 a_position;
out vec2 a_tex;
out vec3 a_color;
out float a_mix;

out vec4 a_shadow;
out vec4 a_shadow2;

void main() {
    a_shadow = u_mvp1 * vec4(in_position, 1.0);
    a_shadow.xyz /= 2.0f;
    a_shadow.xyz += vec3(0.5, 0.5, 0.5);

    a_shadow2 = u_mvp_big * vec4(in_position, 1.0);
    a_shadow2.xyz /= 2.0f;
    a_shadow2.xyz += vec3(0.5, 0.5, 0.5);

    a_normal = vec3(u_mv_normal * vec4(in_normal, 1));
    a_position = vec3(u_model * vec4(in_position, 1.0));
    a_tex = in_tex.xy;
    a_color = in_color;
    a_mix = in_tex.z;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
