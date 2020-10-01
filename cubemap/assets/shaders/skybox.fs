#version 330 core

out vec4 o_frag_color;

in vec3 coord;
uniform samplerCube skybox;

void main() {
    o_frag_color = vec4(texture(skybox, coord).rgb, 1);
}
