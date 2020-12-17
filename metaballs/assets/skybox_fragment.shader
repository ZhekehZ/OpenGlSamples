#version 430 core

out vec4 o_frag_color;

in vec3 coord;
uniform samplerCube u_skybox;

void main() {
    o_frag_color = vec4(texture(u_skybox, coord).rgb, 1);
}
