#version 430 core

uniform vec3 u_camera_center;
uniform samplerCube u_skybox;
uniform float u_refraction_value;

in vec3 v_normal;
in vec3 v_position;

out vec4 o_frag_color;

void main() {
    vec3 view = normalize(v_position - u_camera_center);
    vec3 normal = -v_normal;

    vec3 reflected = reflect(view, normal);
    vec3 refracted = refract(view, normal, 1 / u_refraction_value);
    vec4 reflection = vec4(texture(u_skybox, reflected).rgb, 1.0);
    vec4 refraction = vec4(texture(u_skybox, refracted).rgb, 1.0);
    float a = 1 - dot(view, normal);
    float R0 = (u_refraction_value - 1) * (u_refraction_value - 1)
             / (u_refraction_value + 1) / (u_refraction_value + 1);
    float a_fresnel = R0 + (1 - R0) * a * a * a * a * a;            // Schlick's approximation
    o_frag_color = mix(reflection, refraction, 1);
}
