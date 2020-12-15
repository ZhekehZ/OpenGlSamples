#version 330 core

out vec4 o_frag_color;

uniform sampler1D u_texture;
uniform vec2 scale;

uniform vec2 u_c;
uniform int u_n;
uniform float u_radius;

uniform vec2 u_shift1;
uniform vec2 u_shift2;
uniform float u_zoom;

in vec2 v_out;

vec2 complex_sqr(vec2 a) {
    return vec2(a.x * a.x - a.y * a.y, a.y * a.x + a.x * a.y);
}

void main() {
    vec2 z = ((v_out.xy + u_shift1) * u_zoom + u_shift2) * scale;
    
    int i = 0;
    for (; i <= u_n && length(z) < u_radius; ++i) {
        z = complex_sqr(z) + u_c;
    }

    o_frag_color = texture(u_texture, float(i) / u_n);
}
