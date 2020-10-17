#version 330 core

out vec4 o_frag_color;
in vec3 a_normal;
in vec3 a_position;
in vec2 a_tex;
in vec3 a_color;
in float a_mix;
in vec4 a_shadow;

uniform float time;
uniform sampler2D u_texture;

uniform sampler2D u_shadow;

void main() {
    vec3 sun_position = vec3(0, 0, 10);
    float light1 = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0);
    vec3 shadow = a_shadow.xyz / a_shadow.w;
    float depth = texture(u_shadow, shadow.xy).x;

    float light = (shadow.x < 1 && shadow.x > 0 && shadow.y < 1 && shadow.y > 0 &&
                depth < shadow.z - 0.001) ? light1 / 2 : light1;

    vec3 color = mix(a_color, texture(u_texture, a_tex).rgb, a_mix);
    o_frag_color = vec4(light * color, 1);
}
