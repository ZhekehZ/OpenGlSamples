#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tex;

uniform mat4 u_mvp;
uniform mat4 u_mvp_light_near;
uniform mat4 u_mvp_light_far;
uniform mat4 u_mvp_light_dir;

out vec3 a_position;
out vec3 a_normal;
out vec3 a_tex;

out vec3 a_shadow_near;
out vec3 a_shadow_far;
out vec3 a_shadow_dir;

void main() {
    vec4 shad_far_coord = u_mvp_light_far * vec4(in_position, 1.0);
    a_shadow_far = shad_far_coord.xyz / shad_far_coord.w / 2 + 0.5;

    vec4 shad_near_coord = u_mvp_light_near * vec4(in_position, 1.0);
    a_shadow_near = shad_near_coord.xyz / shad_near_coord.w / 2 + 0.5;

    vec4 shad_dir = u_mvp_light_dir * vec4(in_position, 1.0);
    a_shadow_dir = shad_dir.xyz / shad_dir.w / 2 + 0.5;

    a_normal = in_normal;
    a_position = in_position;
    a_tex = in_tex;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
