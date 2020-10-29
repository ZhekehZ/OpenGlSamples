#version 330 core

in vec3 a_position;
in vec3 a_normal;
in vec3 a_tex;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D u_detail;

uniform mat4 u_mvp_light_near;
uniform mat4 u_mvp_light_far;
uniform mat4 u_mvp_light_dir;

uniform sampler2D u_shadow_near;
uniform sampler2D u_shadow_far;

uniform float u_time;

out vec4 o_frag_color;

bool is_inside(vec3 c) {
    return c.x < 1 && c.x > 0 && c.y < 1 && c.y > 0 && c.z < 1 && c.z > 0;
}

vec3 get_point(mat4 mat, vec4 position) {
    position = mat * position;
    return position.xyz / position.w / 2 + 0.5;
}

float thresh_low = 0.15;
float thresh_high = 0.7;
float thresh_mid = (thresh_low + thresh_high) / 2;
float thresh_size_2 = (thresh_high - thresh_low) / 2;

vec3 sun_position = vec3(0, 0, 10);
vec3 ambient = vec3(0.03, 0.03, 0.03);
vec3 direct = vec3(1, 1, 0.5);

float get_shadow(sampler2D shadow, vec3 coord, float bias) {
    return texture(shadow, coord.xy).x < coord.z - bias ? 0.1 : 1;
}

void main() {
    vec4 position = vec4(a_position, 1.0);
    vec3 shadow_far = get_point(u_mvp_light_far, position);
    vec3 shadow_near = get_point(u_mvp_light_near, position);
    vec3 shadow_dir = get_point(u_mvp_light_dir, position);

    bool is_near = is_inside(shadow_near);
    bool is_direct = is_inside(shadow_dir);
    float direct_power = length(shadow_dir - vec3(0.5, 0.5, 0)) / 1.1;

    float simple_light = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0.1);
    float global_light = is_near ? min(get_shadow(u_shadow_near, shadow_near, 0.02), simple_light)
                                 : min(get_shadow(u_shadow_far, shadow_far, 0.03), simple_light);
    float direct_light = is_direct ? max(0, 1 - direct_power) : 0;

    vec2 tex1_coord = a_tex.xy + vec2(sin(u_time) * a_tex.z, a_tex.z);
    tex1_coord -= floor(tex1_coord);
    
    float z = gl_FragCoord.z / gl_FragCoord.w;
    vec3 details = mix(texture(u_detail, a_tex.xy).rgb, vec3(1, 1, 1), z);

    vec3 color1 = texture(texture1, tex1_coord).rgb;
    vec3 color2 = texture(texture2, a_tex.xy).rgb;
    vec3 color3 = texture(texture3, a_tex.xy).rgb * details;

    float alpha = min(1, max(0, abs(thresh_mid - a_tex.z) / thresh_size_2));
    vec3 diffuse = a_tex.z < thresh_mid - 0.01
               ? color1
               : a_tex.z < thresh_mid ? vec3(0.1, 0.1, 0.1)
               : mix(color2, color3, pow(alpha, 5));

    diffuse = diffuse * (global_light + direct_light) + direct * direct_light / 2;
    o_frag_color = vec4(diffuse + ambient, 1);
}
