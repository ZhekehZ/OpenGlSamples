#version 330 core

out vec4 o_frag_color;

in vec2 v_out;

void main() {
    o_frag_color = vec4(v_out.x, v_out.y, 0, 1);
}
