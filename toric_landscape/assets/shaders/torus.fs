#version 330 core

in vec3 a_position;
in vec3 a_normal;
in vec3 a_tex;
in vec3 a_shadow_near;
in vec3 a_shadow_far;
in vec3 a_shadow_dir;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform sampler2D u_shadow_near;
uniform sampler2D u_shadow_far;
uniform sampler2D u_shadow_dir;

out vec4 o_frag_color;

bool is_inside(vec3 c) {
    return c.x < 1 && c.x > 0 && c.y < 1 && c.y > 0 && c.z < 1 && c.z > 0;
}

float thresh_low = 0.3;
float thresh_high = 0.5;
float thresh_mid = (thresh_low + thresh_high) / 2;
float thresh_size_2 = (thresh_high - thresh_low) / 2;

void main() {
    vec3 sun_position = vec3(0, 0, 10);
    float light1 = max(dot(normalize(sun_position - a_position), normalize(a_normal)), 0);

    float depth_near = texture(u_shadow_near, a_shadow_near.xy).x;
    float depth_far = texture(u_shadow_far, a_shadow_far.xy).x;
    float depth_dir = texture(u_shadow_dir, a_shadow_dir.xy).x;

    bool is_near = is_inside(a_shadow_near);
    bool is_direct = is_inside(a_shadow_dir);

    float light2 = is_near ? (depth_near < (a_shadow_near.z - 0.01) ? min(0.2, light1) : light1)
                           : (depth_far  < (a_shadow_far.z - 0.015) ? min(0.2, light1) : light1);

    float light = (is_direct && depth_dir < a_shadow_dir.z - 0.00001) ? light2 + 0.5 : light2;

    vec3 color1 = texture(texture1, a_tex.xy).rgb;
    vec3 color2 = texture(texture2, a_tex.xy).rgb;
    vec3 color3 = texture(texture3, a_tex.xy).rgb;

    float alpha = min(1, max(0, abs(thresh_mid - a_tex.z) / thresh_size_2));
    vec3 color = a_tex.z < thresh_mid
               ? mix(color2, color1, 1 - pow(1 - alpha, 5))
               : mix(color2, color3, pow(alpha, 5));

    o_frag_color = vec4(light * color, 1);
}
