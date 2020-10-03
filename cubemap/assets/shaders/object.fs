#version 330 core

out vec4 o_frag_color;

in vec3 a_reflected;
in vec3 a_refracted;
in vec4 a_color;
in vec2 a_tex_coords;

uniform samplerCube u_skybox;
uniform sampler2D u_texture;
uniform float u_reflectivity;
uniform float u_color_intensity;

uniform float is_textured; // 1.0f or 0.0f

void main() {
    vec4 reflection = vec4(texture(u_skybox, a_reflected).rgb, 1.0);
    vec4 refraction = vec4(texture(u_skybox, a_refracted).rgb, 1.0);

    vec4 tex_clr = mix(a_color, texture(u_texture, a_tex_coords) * a_color, is_textured);
    vec4 effects = mix(refraction, reflection, u_reflectivity);
    o_frag_color = mix(effects, tex_clr, u_color_intensity);
}
