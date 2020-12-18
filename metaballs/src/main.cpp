#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <imgui.h>

#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

#include "shader/opengl_shader.h"
#include "utils/error_callbacks.h"
#include "utils/texture_loader.h"
#include "data/cube_data.h"

#include "state.h"
#include "constants.h"

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

    // Load assets
    shader_t shader("assets/shaders/metaballs/vertex.shader",
                    "assets/shaders/metaballs/fragment.shader",
                    "assets/shaders/metaballs/geometry.shader");
    shader_t skybox_shader("assets/shaders/skybox/vertex.shader",
                           "assets/shaders/skybox/fragment.shader",
                           "");
    GLuint skybox_texture = load_cube_texture({
        "assets/skybox/right.png",
        "assets/skybox/left.png",
        "assets/skybox/up.png",
        "assets/skybox/down.png",
        "assets/skybox/front.png",
        "assets/skybox/back.png"
    });

    // Init state
    state_t state;

    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        float ratio = (float) display_w / (float) display_h;
        glViewport(0, 0, display_w, display_h);

        // ImGui interface render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");
        ImGui::SliderFloat("Refraction coeff.", &state.refraction_value, 1, 5);
        ImGui::SliderInt("Balls", &state.n_balls, 1, 6);
        auto [fps, frame_time] = state.get_fps_and_frame_time();
        ImGui::LabelText("", "Frame time: %dms", frame_time);
        ImGui::LabelText("", "FPS       : %d", fps);
        ImGui::Checkbox("VSync", &state.vsync);
        ImGui::End();

        // Handle mouse events
        if (!ImGui::IsAnyWindowFocused()) {
            auto [dx, dy] = ImGui::GetMouseDragDelta();
            ImGui::ResetMouseDragDelta();

            float ax = dx / (float) display_w;
            float ay = dy / (float) display_h;

            state.forward = glm::rotate(state.forward, ax, state.up);
            glm::vec3 right = glm::cross(state.forward, state.up);
            state.forward = glm::rotate(state.forward, ay, right);
            state.up = glm::rotate(state.up, ay, right);
        }

        float wheel = ImGui::GetIO().MouseWheel;
        if (abs(wheel) > 0.1) {
            wheel = wheel > 0 ? 1/1.2f : 1.2f;
            state.zoom *= wheel;
        }

        // OpenGL render
        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(unsigned(GL_COLOR_BUFFER_BIT) | unsigned(GL_DEPTH_BUFFER_BIT));
        glEnable(GL_DEPTH_TEST);


        // Metaballs render
        glm::mat4 model = glm::mat4(1);
        glm::mat4 view = glm::lookAt(-state.forward * state.zoom, glm::vec3(0, 0, 0), state.up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), ratio, 0.05f, 20.0f);
        glm::mat4 mv = view * model;
        glm::mat4 mvp = projection * mv;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

        shader.use();
        shader.set_uniform("MVP", glm::value_ptr(mvp));
        shader.set_uniform("hedge_size", edge_size / 2);
        shader.set_uniform("u_camera_center",
                           -state.forward.x * state.zoom,
                           -state.forward.y * state.zoom,
                           -state.forward.z * state.zoom);
        shader.set_uniform("u_refraction_value", state.refraction_value);
        shader.set_uniform("radius", 0.1f);
        shader.set_uniform("n_metaballs", state.n_balls);
        for (int i = 0; i < state.n_balls; ++i) {
            std::stringstream ss;
            ss << "metaball" << i + 1;
            shader.set_uniform(ss.str(),
                               state.meta_balls[i].x,
                               state.meta_balls[i].y,
                               state.meta_balls[i].z);
        }
        shader.set_uniform("u_skybox", 0);

        glBindVertexArray(state.grid_vao);
        glDrawArrays(GL_POINTS, 0, grid_points_count);
        glBindVertexArray(0);

        // Skybox render
        glm::mat4 view_skybox = glm::lookAt(-state.forward, glm::vec3(0, 0, 0), state.up);
        auto mvp_skybox = projection * view_skybox;

        skybox_shader.use();
        skybox_shader.set_uniform("u_skybox", 0);
        skybox_shader.set_uniform("u_mvp", glm::value_ptr(mvp_skybox));

        glBindVertexArray(state.skybox_vao);
        glDrawElements(GL_TRIANGLES, cubeFSize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // Update state
        state.update();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
