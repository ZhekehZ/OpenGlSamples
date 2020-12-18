#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <chrono>
#include <random>

#include "data/cube_data.h"
#include "data/marching_cubes_table.h"
#include "constants.h"

struct state_t {

    state_t() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> dist(-1, 1);

        for (auto & meta_balls_dir : meta_balls_dirs) {
            meta_balls_dir.x = dist(e2);
            meta_balls_dir.y = dist(e2);
            meta_balls_dir.z = dist(e2);
            meta_balls_dir = normalize(meta_balls_dir);
        }

        init_grid();
        init_skybox();
        load_marching_cubes_data();

        frame_start_time = std::chrono::high_resolution_clock::now();
        prev_update = frame_start_time;
    }

    void init_grid() {
        std::vector<float> V;

        for (int i = -grid_size; i <= grid_size; i++) {
            for (int j = -grid_size; j <= grid_size; j++) {
                for (int k = -grid_size; k <= grid_size; k++) {
                    V.push_back(float(i) * edge_size);
                    V.push_back(float(j) * edge_size);
                    V.push_back(float(k) * edge_size);
                }
            }
        }

        GLuint vbo;
        glGenVertexArrays(1, &grid_vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(grid_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V[0]) * V.size(), V.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void init_skybox() {
        GLuint vbo, ebo;
        glGenVertexArrays(1, &skybox_vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(skybox_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, cubeVSize * sizeof(float), cubeV, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeFSize * sizeof(int), cubeF, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    static void load_marching_cubes_data() {
        GLuint ubo;
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ubo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(triangle_table), triangle_table, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ubo);
    }

    std::pair<int, int> get_fps_and_frame_time() {
        return {display_fps, display_frame_time};
    }

    void update() {
        if (curr_vsync != vsync) {
            glfwSwapInterval(vsync);
            curr_vsync = vsync;
        }

        for (int i = 0; i < metaballs_count; i++) {
            meta_balls[i] = 0.8f * ((float) sin(glfwGetTime())) * meta_balls_dirs[i];
        }

        std::chrono::duration<double, std::milli> frame_time =
                std::chrono::high_resolution_clock::now() - frame_start_time;
        frame_delta_time_ms = frame_time.count();

        if (frame_start_time - prev_update > std::chrono::milliseconds(500)) {
            display_fps = 1000.0 / (frame_delta_time_ms + 1e-10);
            display_frame_time = frame_delta_time_ms;
            prev_update = frame_start_time;
        }

        frame_start_time = std::chrono::high_resolution_clock::now();
    }

    GLuint grid_vao{}, skybox_vao{};
    glm::vec3 meta_balls[metaballs_count] = {};
    glm::vec3 forward = {1, 0, 0};
    glm::vec3 up = {0, 1, 0};
    float zoom = 1;
    int n_balls = 0;
    bool vsync = true, curr_vsync = false;
    float refraction_value = 1.33f;

private:
    glm::vec3 meta_balls_dirs[metaballs_count] = {};

    std::chrono::high_resolution_clock::time_point frame_start_time;
    std::chrono::high_resolution_clock::time_point prev_update = frame_start_time;
    double frame_delta_time_ms = 0;

    double display_fps = 0;
    double display_frame_time = 0;
};