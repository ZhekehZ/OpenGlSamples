#version 330 core

out vec4 o_frag_color;

in vec3 reflected;
in vec3 refracted;
in vec4 color;
in vec2 tex;

uniform samplerCube skybox;
uniform sampler2D u_texture;
uniform float reflectivity;
uniform float color_intensity;

uniform float is_textured;

void main() {
    vec4 reflection = vec4(texture(skybox, reflected).rgb, 1.0);
    vec4 refraction = vec4(texture(skybox, refracted).rgb, 1.0);

    vec4 col = is_textured * texture(u_texture, tex) * color + (1 - is_textured) * color;

    o_frag_color = color_intensity * col +
                   (1 - color_intensity) * (
                        (1 - reflectivity) * refraction +
                        reflectivity * reflection
                   );

}
