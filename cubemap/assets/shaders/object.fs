#version 330 core

out vec4 o_frag_color;

in vec4 a_color;
in vec2 a_tex_coords;

in vec3 view;
in vec3 norm;

uniform samplerCube u_skybox;
uniform sampler2D u_texture;
uniform float u_color_intensity;

uniform float is_textured; // 1.0f or 0.0f
uniform float u_refraction_value;

void main() {
    vec3 reflected = reflect(view, norm);
    vec3 refracted = refract(view, norm, 1 / u_refraction_value);

    vec4 reflection = vec4(texture(u_skybox, reflected).rgb, 1.0);
    vec4 refraction = vec4(texture(u_skybox, refracted).rgb, 1.0);

    float a = 1 + dot(view, norm);
    float R0 = (u_refraction_value - 1) * (u_refraction_value - 1)
             / (u_refraction_value + 1) / (u_refraction_value + 1);
    float a_fresnel = R0 + (1 - R0) * a * a * a * a * a;            // Schlick's approximation

    vec4 tex_clr = mix(a_color, texture(u_texture, a_tex_coords) * a_color, is_textured);
    vec4 effects = mix(refraction, reflection, a_fresnel);
    o_frag_color = mix(effects, tex_clr, u_color_intensity);
}
