#version 330 core

out vec4 o_frag_color;
in vec3 a_normal;
in vec3 a_position;
in vec3 a_tex;
in vec3 a_shadow;

uniform float time;
uniform sampler2D t1;
uniform sampler2D t2;
uniform sampler2D t3;

uniform sampler2D u_shadow;

void main() {
    //vec3 sun_position = vec3(cos(time) * 2 + 2, 0, sin(time) * 2);
    //float light = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0.3);
    float light = texture(u_shadow, a_shadow.xy / a_shadow.z).x < a_shadow.z ? 1.0 : 0.3;

    vec3 color1 = texture(t1, a_tex.xy).rgb;
    vec3 color2 = texture(t2, a_tex.xy).rgb;
    vec3 color3 = texture(t3, a_tex.xy).rgb;
    vec3 color = a_tex.z < 0.35 ? color1 :
                 a_tex.z < 0.4 ? mix(color1, color2, (a_tex.z - 0.35) * 20) :
                 a_tex.z < 0.6 ? color2 :
                 a_tex.z < 0.65 ? mix(color2, color3, (a_tex.z - 0.6) * 20) :
                 color3;

    o_frag_color = vec4(color * light, 1);
}
