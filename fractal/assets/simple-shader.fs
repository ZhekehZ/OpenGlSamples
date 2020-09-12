#version 330 core

out vec4 o_frag_color;

uniform sampler1D u_texture;
uniform vec2 u_c;
uniform int u_n;
uniform float u_radius;

uniform vec2 u_shift1;
uniform vec2 u_shift2;
uniform float u_zoom;

in vec2 v_out;

vec2 c_mult(vec2 a, vec2 b) {
  return vec2(
    a.x * b.x - a.y * b.y,
    a.y * b.x + a.x * b.y
  );
}

void main() {
    vec2 z = (v_out.xy + u_shift1) * u_zoom + u_shift2;

    float color = 0;
    for (int i = 0; i <= u_n && length(z) < u_radius; ++i) {
        z = c_mult(z, z) + u_c;
        color += 1.0 / u_n;
    }

    o_frag_color = texture1D(u_texture, color);
}
