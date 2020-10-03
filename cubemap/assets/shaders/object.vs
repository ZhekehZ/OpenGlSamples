#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec4 in_color;
layout (location = 3) in vec2 in_tex;

out vec3 reflected;
out vec3 refracted;
out vec4 color;
out vec2 tex;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_model_normal;
uniform vec3 camera;

uniform float refraction_value;

void main() {
    vec3 view = normalize((u_model * vec4(in_position, 1.0)).xyz - camera);
    vec3 u_norm = normalize((u_model_normal * vec4(in_normal, 1.0)).xyz);

    reflected = reflect(view, u_norm);
    refracted = refract(view, u_norm, refraction_value);
    color = in_color;
    tex = in_tex;

    gl_Position = u_mvp * vec4(in_position, 1.0);
}
