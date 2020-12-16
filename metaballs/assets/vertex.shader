#version 430 core
layout (location = 0) in vec3 in_position;

out vs_to_gs_t {
    float f;
} vs_out;

void main() {
    gl_Position = vec4(in_position, 1.0);
}