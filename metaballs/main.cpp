#include <iostream>
#include <random>
#include <sstream>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include "opengl_shader.h"
#include "cube_data.h"
#include "texture_loader.h"
#include "error_callbacks.h"
#include "marching_cubes_table.h"

static std::vector<float> V;
static float edge_size = 0.02;

static glm::vec3 meta_balls[6] = {};
static glm::vec3 meta_balls_dirs[6] = {};

void init_buffers(GLuint &vbo, GLuint &vao, GLuint &ebo, GLuint &tex) {
    for (int i = -50; i <= 50; i++) {
        for (int j = -50; j <= 50; j++) {
            for (int k = -50; k <= 50; k++) {
                V.push_back(float(i) * edge_size);
                V.push_back(float(j) * edge_size);
                V.push_back(float(k) * edge_size);
            }
        }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V[0]) * V.size(), V.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint init_skybox_vertex_buffer(float *v_data, int v_size, int *i_data, int i_size) {
    GLuint vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v_size * sizeof(float), v_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_size * sizeof(int), i_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

void load_marching_cubes_data() {
    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ubo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(triangle_table), triangle_table, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ubo);
}

int main(int, char **) {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) return 1;
    const char *glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1200, 800, "MetaBalls", nullptr, nullptr);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);

    GLuint vbo, vao, ebo, tex;
    init_buffers(vbo, vao, ebo, tex);

    shader_t shader("assets/vertex.shader",
                    "assets/fragment.shader",
                    "assets/geometry.shader");

    GLuint skybox_vao = init_skybox_vertex_buffer(cubeV, cubeVSize, cubeF, cubeFSize);
    shader_t skybox_shader("assets/skybox_vertex.shader",
                           "assets/skybox_fragment.shader",
                           "");

    GLuint skybox_texture = load_cube_texture({
        "assets/right.png",
        "assets/left.png",
        "assets/up.png",
        "assets/down.png",
        "assets/front.png",
        "assets/back.png"
    });

    load_marching_cubes_data();

    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    glm::vec3 forward = {1, 0, 0};
    glm::vec3 up = {0, 1, 0};

    float zoom = 1;


    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 10);

    for (auto & meta_balls_dir : meta_balls_dirs) {
        meta_balls_dir = glm::rotate(glm::vec3(1, 0, 0), (float) dist(e2), glm::vec3(0, 1, 0));
        meta_balls_dir = glm::rotate(meta_balls_dir, (float) dist(e2), glm::vec3(0, 0, 1));
    }

    auto frame_start_time = std::chrono::high_resolution_clock::now();
    auto prev_update = frame_start_time;
    int frame_delta_time_ms = 0;
    int display_fps = 0, display_frame_time = 0;

    int n_balls = 0;
    bool vsync = true, curr_vsync = false;

    while (!glfwWindowShouldClose(window)) {
        frame_start_time = std::chrono::high_resolution_clock::now();
        if (curr_vsync != vsync) {
            glfwSwapInterval(vsync);
            curr_vsync = vsync;
        }

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        float ratio = (float) display_w / (float) display_h;
        glViewport(0, 0, display_w, display_h);


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");
        ImGui::SliderInt("Balls", &n_balls, 1, 6);
        if (frame_start_time - prev_update > std::chrono::milliseconds(500)) {
            display_fps = 1000 / (frame_delta_time_ms + 1);
            display_frame_time = frame_delta_time_ms;
            prev_update = frame_start_time;
        }
        ImGui::LabelText("", "Frame time: %dms", display_frame_time);
        ImGui::LabelText("", "FPS       : %d", display_fps);
        ImGui::Checkbox("VSync", &vsync);
        ImGui::End();


        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (!ImGui::IsAnyWindowFocused()) {
            auto [dx, dy] = ImGui::GetMouseDragDelta();
            ImGui::ResetMouseDragDelta();

            float ax = dx / (float) display_w;
            float ay = dy / (float) display_h;

            forward = glm::rotate(forward, ax, up);
            glm::vec3 right = glm::cross(forward, up);
            forward = glm::rotate(forward, ay, right);
            up = glm::rotate(up, ay, right);
        }

        float wheel = ImGui::GetIO().MouseWheel;
        if (abs(wheel) > 0.1) {
            wheel = wheel > 0 ? 1/1.2f : 1.2f;
            zoom *= wheel;
        }

        glm::mat4 model = glm::mat4(1);
        glm::mat4 view = glm::lookAt(-forward * zoom, glm::vec3(0, 0, 0), up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), ratio, 0.05f, 20.0f);
        glm::mat4 mv = view * model;
        glm::mat4 mvp = projection * mv;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

        shader.use();
        shader.set_uniform("MVP", glm::value_ptr(mvp));
        shader.set_uniform("hedge_size", edge_size / 2);
        shader.set_uniform("u_view_dir", forward.x, forward.y, forward.z);
        shader.set_uniform("radius", 0.1f);
        shader.set_uniform("n_metaballs", n_balls);
        for (int i = 0; i < n_balls; ++i) {
            std::stringstream ss;
            ss << "metaball" << i + 1;
            shader.set_uniform(ss.str(), meta_balls[i].x, meta_balls[i].y, meta_balls[i].z);
        }
        shader.set_uniform("u_skybox", 0);

        for (int i = 0; i < 6; i++) {
            meta_balls[i] = 0.8f * ((float) sin(glfwGetTime())) * meta_balls_dirs[i];
        }

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, V.size() / 3);
        glBindVertexArray(0);

        glm::mat4 view_skybox = glm::lookAt(-forward, glm::vec3(0, 0, 0), up);
        auto mvp_skybox = projection * view_skybox;

        skybox_shader.use();
        skybox_shader.set_uniform("u_skybox", 0);
        skybox_shader.set_uniform("u_mvp", glm::value_ptr(mvp_skybox));

        glBindVertexArray(skybox_vao);
        glDrawElements(GL_TRIANGLES, cubeFSize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();

        std::chrono::duration<double, std::milli>
                frame_time = std::chrono::high_resolution_clock::now() - frame_start_time;
        frame_delta_time_ms = static_cast<int>(frame_time.count());
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
