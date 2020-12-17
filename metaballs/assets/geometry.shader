#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 15) out;

uniform mat4 MVP;
uniform float hedge_size;

uniform vec3 metaball1;
uniform vec3 metaball2;
uniform vec3 metaball3;
uniform vec3 metaball4;
uniform vec3 metaball5;
uniform vec3 metaball6;

uniform int n_metaballs = 2;

uniform float radius;

out vec3 normal;

// data & algorithm source: http://paulbourke.net/geometry/polygonise/
layout (std430, binding = 2) buffer triangle_table_t {
    int triangle_table[];
};

int edge_table[256] = {
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

float sqr_dist(vec3 position, vec3 center) {
    float dist = distance(position, center);
    return dist * dist;
}

float F(vec3 position) {
    float sum = 0;
    if (n_metaballs > 0) sum += 1 / sqr_dist(position, metaball1);
    if (n_metaballs > 1) sum += 1 / sqr_dist(position, metaball2);
    if (n_metaballs > 2) sum += 1 / sqr_dist(position, metaball3);
    if (n_metaballs > 3) sum += 1 / sqr_dist(position, metaball4);
    if (n_metaballs > 4) sum += 1 / sqr_dist(position, metaball5);
    if (n_metaballs > 5) sum += 1 / sqr_dist(position, metaball6);
    return radius * radius * sum - 1;
}

vec3 interpolate(vec3 point1, vec3 point2, float value1, float value2) {
    return point1 + (point2 - point1) * abs(value1) / (abs(value1) + abs(value2));
}

vec3 get_normal (vec3 position) {
    return normalize(vec3(
        F(position + vec3(hedge_size / 4, 0, 0)) - F(position - vec3(hedge_size / 4, 0, 0)),
        F(position + vec3(0, hedge_size / 4, 0)) - F(position - vec3(0, hedge_size / 4, 0)),
        F(position + vec3(0, 0, hedge_size / 4)) - F(position - vec3(0, 0, hedge_size / 4))
    ));
}

void produce(vec3 center) {
    vec3 points[12] = {
        vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0),
        vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0),
        vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0)
    };

    vec3 cube[8] = {
        center + vec3(-hedge_size, -hedge_size, +hedge_size),
        center + vec3(+hedge_size, -hedge_size, +hedge_size),
        center + vec3(+hedge_size, -hedge_size, -hedge_size),
        center + vec3(-hedge_size, -hedge_size, -hedge_size),
        center + vec3(-hedge_size, +hedge_size, +hedge_size),
        center + vec3(+hedge_size, +hedge_size, +hedge_size),
        center + vec3(+hedge_size, +hedge_size, -hedge_size),
        center + vec3(-hedge_size, +hedge_size, -hedge_size),
    };

    float fs[8] = {
        F(cube[0]), F(cube[1]), F(cube[2]), F(cube[3]),
        F(cube[4]), F(cube[5]), F(cube[6]), F(cube[7])
    };

    int index = 0;
    if (fs[0] < 0) index |= 1;
    if (fs[1] < 0) index |= 2;
    if (fs[2] < 0) index |= 4;
    if (fs[3] < 0) index |= 8;
    if (fs[4] < 0) index |= 16;
    if (fs[5] < 0) index |= 32;
    if (fs[6] < 0) index |= 64;
    if (fs[7] < 0) index |= 128;

    int edges = edge_table[index];

    if (edges == 0) return;

    if ((edges & 1) != 0) points[0] = interpolate(cube[0], cube[1], fs[0], fs[1]);
    if ((edges & 2) != 0) points[1] = interpolate(cube[1], cube[2], fs[1], fs[2]);
    if ((edges & 4) != 0) points[2] = interpolate(cube[2], cube[3], fs[2], fs[3]);
    if ((edges & 8) != 0) points[3] = interpolate(cube[3], cube[0], fs[3], fs[0]);
    if ((edges & 16) != 0) points[4] = interpolate(cube[4], cube[5], fs[4], fs[5]);
    if ((edges & 32) != 0) points[5] = interpolate(cube[5], cube[6], fs[5], fs[6]);
    if ((edges & 64) != 0) points[6] = interpolate(cube[6], cube[7], fs[6], fs[7]);
    if ((edges & 128) != 0) points[7] = interpolate(cube[7], cube[4], fs[7], fs[4]);
    if ((edges & 256) != 0) points[8] = interpolate(cube[0], cube[4], fs[0], fs[4]);
    if ((edges & 512) != 0) points[9] = interpolate(cube[1], cube[5], fs[1], fs[5]);
    if ((edges & 1024) != 0) points[10] = interpolate(cube[2], cube[6], fs[2], fs[6]);
    if ((edges & 2048) != 0) points[11] = interpolate(cube[3], cube[7], fs[3], fs[7]);

    index *= 15;

    for (int i = 0; i < 16 && triangle_table[index + i] != -1; i += 3) {
        gl_Position = MVP * vec4(points[triangle_table[index + i]], 1);
        normal = get_normal(points[triangle_table[index + i]]);
        EmitVertex();
        gl_Position = MVP * vec4(points[triangle_table[index + i + 1]], 1);
        normal = get_normal(points[triangle_table[index + i + 1]]);
        EmitVertex();
        gl_Position = MVP * vec4(points[triangle_table[index + i + 2]], 1);
        normal = get_normal(points[triangle_table[index + i + 2]]);
        EmitVertex();
        EndPrimitive();
    }
}


void main() {
    produce(gl_in[0].gl_Position.xyz);
}