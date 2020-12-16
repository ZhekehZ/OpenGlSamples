#version 430 core

uniform vec3 u_view_dir;

uniform samplerCube u_skybox;

float u_refraction_value = 1000;

in v_data_t {
    vec3 normal;
} v_data;

out vec4 o_frag_color;

void main() {
    vec3 reflected = reflect(u_view_dir, v_data.normal);
    vec3 refracted = refract(u_view_dir, v_data.normal, 1 / u_refraction_value);
    vec4 reflection = vec4(texture(u_skybox, reflected).rgb, 1.0);
    vec4 refraction = vec4(texture(u_skybox, refracted).rgb, 1.0);
    float a = 1 - dot(u_view_dir, v_data.normal);
    float R0 = (u_refraction_value - 1) * (u_refraction_value - 1)
             / (u_refraction_value + 1) / (u_refraction_value + 1);
    float a_fresnel = R0 + (1 - R0) * a * a * a * a * a;            // Schlick's approximation
    o_frag_color = mix(reflection, refraction, 1);
}
