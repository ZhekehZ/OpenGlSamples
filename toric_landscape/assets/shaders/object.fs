#version 330 core

in vec3 a_normal;
in vec3 a_position;
in vec3 a_tex;
in vec3 a_color;
in vec3 a_shadow;

uniform sampler2D u_texture;
uniform sampler2D u_shadow;

out vec4 o_frag_color;

void main() {
    vec3 sun_position = vec3(0, 0, 10);
    float light1 = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0);
    float shadow_depth = texture(u_shadow, a_shadow.xy).x;
    float light = shadow_depth < (a_shadow.z - 0.01) ? light1 / 2 : light1;

    vec3 color = mix(a_color, texture(u_texture, a_tex.xy).rgb, a_tex.z);
    o_frag_color = vec4(light * color, 1);
}
