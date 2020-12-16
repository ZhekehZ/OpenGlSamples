#include <iostream>
#include <chrono>
#include <fmt/format.h>
#include <GL/glew.h>
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "opengl_shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

static void glfw_error_callback(int error, const char *description) {
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

inline void print_OGL_errors() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << gluErrorString(err) << std::endl;
    }
}

std::vector<float> V;

void init_buffers(GLuint &vbo, GLuint &vao, GLuint &ebo, GLuint &tex) {
    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            for (int k = -10; k <= 10; k++) {
                V.push_back(i * 0.1);
                V.push_back(j * 0.1);
                V.push_back(k * 0.1);
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
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    GLuint vbo, vao, ebo, tex;
    init_buffers(vbo, vao, ebo, tex);

    shader_t shader("vertex.shader",
                    "fragment.shader",
                    "geometry.shader");

    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)) {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");
        ImGui::End();

        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glm::mat4 model = glm::mat4(1);
        glm::mat4 view = glm::lookAt(
                glm::vec3(0.7, 0.7, 0.7),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(
                45.0f, (float) display_w / (float) display_h
                , 0.05f, 2.0f);

        glm::mat4 mvp = projection * view * model;

        float * data = glm::value_ptr(mvp);
        shader.set_uniform("MVP", data);
        shader.set_uniform("hedge_size", 0.05f);
        shader.use();

        glPointSize(12);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, V.size() / 3);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        print_OGL_errors();
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0; 
}
