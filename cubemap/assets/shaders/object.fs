#version 330 core

out vec4 o_frag_color;

in vec3 reflected;
in vec3 refracted;
in vec4 color;

uniform samplerCube skybox;
uniform float reflectivity;
uniform float color_intensity;

void main() {
    vec4 reflection = vec4(texture(skybox, reflected).rgb, 1.0);
    vec4 refraction = vec4(texture(skybox, refracted).rgb, 1.0);

    o_frag_color = color_intensity * color +
                   (1 - color_intensity) * (
                        (1 - reflectivity) * refraction +
                        reflectivity * reflection
                   );
}
