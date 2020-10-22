#version 330 core

out vec4 o_frag_color;
in vec3 a_normal;
in vec3 a_position;
in vec3 a_tex;
in vec4 a_shadow;
in vec4 a_shadow2;
in vec4 a_shadow3;

uniform float time;
uniform sampler2D t1;
uniform sampler2D t2;
uniform sampler2D t3;

uniform sampler2D u_shadow;
uniform sampler2D u_shadow2;
uniform sampler2D u_shadow3;


void main() {
    vec3 sun_position = vec3(0, 0, 10);
    float light1 = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0);

    vec3 shadow = a_shadow.xyz / a_shadow.w;
    float depth = texture(u_shadow, shadow.xy).x;

    vec3 shadow2 = a_shadow2.xyz / a_shadow2.w;
    float depth2 = texture(u_shadow2, shadow2.xy).x;

    vec3 shadow3 = a_shadow3.xyz;
    float depth3 = texture(u_shadow3, shadow3.xy).x;

    bool is_near = shadow.x < 1 && shadow.x > 0 && shadow.y < 1 && shadow.y > 0;
    bool is_lig = shadow3.x < 1 && shadow3.x > 0 && shadow3.y < 1 && shadow3.y > 0 && shadow3.z < 1 && shadow3.z > 0;

    float light2 = is_near ? (depth  < ( shadow.z - 0.01) ? min(0.2, light1)  : light1)
                           : (depth2 < (shadow2.z - 0.015) ? min(0.2, light1) : light1);

    float light = (is_lig && depth3 < shadow3.z) ? light2 + 0.5 : light2;

    vec3 color1 = texture(t1, a_tex.xy).rgb;
    vec3 color2 = texture(t2, a_tex.xy).rgb;
    vec3 color3 = texture(t3, a_tex.xy).rgb;
    vec3 color = a_tex.z < 0.35 ? color1 :
                 a_tex.z < 0.4 ? mix(color1, color2, (a_tex.z - 0.35) * 20) :
                 a_tex.z < 0.6 ? color2 :
                 a_tex.z < 0.65 ? mix(color2, color3, (a_tex.z - 0.6) * 20) :
                 color3;

    o_frag_color = vec4(light * color, 1);
}
