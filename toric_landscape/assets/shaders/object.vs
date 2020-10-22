#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_mvp1;
uniform mat4 u_mvp_big;
uniform mat4 u_mvp_light1;

out vec4 a_shadow;
out vec4 a_shadow2;
out vec4 a_shadow3;

out vec3 a_normal;
out vec3 a_position;
out vec3 a_tex;

void main() {
    a_shadow = u_mvp1 * vec4(in_position, 1.0);
    a_shadow.xyz /= 2.0f;
    a_shadow.xyz += vec3(0.5, 0.5, 0.5);


    a_shadow2 = u_mvp_big * vec4(in_position, 1.0);
    a_shadow2.xyz /= 2.0f;
    a_shadow2.xyz += vec3(0.5, 0.5, 0.5);


    a_shadow3 = u_mvp_light1 * vec4(in_position, 1.0);
    a_shadow3 /= a_shadow3.w;
    a_shadow3.xyz /= 2.0;
    a_shadow3.xyz += 0.5;


    a_normal = in_normal;
    a_position = in_position;
    a_tex = in_tex;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
