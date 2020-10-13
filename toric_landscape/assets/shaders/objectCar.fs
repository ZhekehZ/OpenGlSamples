#version 330 core

out vec4 o_frag_color;
in vec3 a_normal;
in vec3 a_position;
in vec2 a_tex;
in vec3 a_color;
in float a_mix;

uniform float time;
uniform sampler2D u_texture;

void main() {
    vec3 sun_position = vec3(0, 0, 0);
    float light = dot(normalize(sun_position - a_position), normalize(a_normal)) + 0.3;

    vec3 color = mix(a_color, texture(u_texture, a_tex).rgb, a_mix);
    o_frag_color = vec4(light * color, 1);
}
