#version 330 core

in vec3 a_normal;
in vec3 a_position;
in vec3 a_tex;
in vec3 a_color;
in vec3 a_shadow;
in vec3 a_position_raw;

uniform sampler2D u_texture;
uniform sampler2D u_shadow;

uniform mat4 u_mvp_light_near;

out vec4 o_frag_color;

vec3 get_point(mat4 mat, vec4 position) {
    position = mat * position;
    return position.xyz / position.w / 2 + 0.5;
}

vec3 sun_position = vec3(0, 0, 10);
vec3 ambient = vec3(0.04, 0.04, 0.04);

void main() {
    vec4 position = vec4(a_position_raw, 1.0);
    vec3 shadow_near = get_point(u_mvp_light_near, position);

    float simple_light = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0);
    float shadow_depth = texture(u_shadow, shadow_near.xy).x;
    float light = shadow_depth < (shadow_near.z - 0.001) ? simple_light / 2 : simple_light;

    vec3 diffuse = mix(a_color, texture(u_texture, a_tex.xy).rgb, a_tex.z);
    o_frag_color = vec4(light * diffuse + ambient, 1);
}
