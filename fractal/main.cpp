#include <iostream>
#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>

#include "opengl_shader.h"

static void glfw_error_callback(int error, const char *description) {
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

float V[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,
};

unsigned int F[] = {0, 1, 2, 2, 3, 0 };

float T[] = {   0/255.0f,   0/255.0f,   0/255.0f,
              255/255.0f,  24/255.0f,  24/255.0f,
              255/255.0f, 248/255.0f,   0/255.0f,
                0/255.0f,   9/255.0f,  80/255.0f
            };
const unsigned NT = sizeof(T) / (3 * sizeof(float));

void init_buffers(GLuint &vbo, GLuint &vao, GLuint &ebo, GLuint &tex) {
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(F), F, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   glGenTextures(1, &tex);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_1D, tex);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, NT, 0, GL_RGB, GL_FLOAT, T);
   glBindTexture(GL_TEXTURE_1D, 0);

}

int main(int, char **) {
    // Use GLFW to create a simple window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // GL 3.3 + GLSL 330
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1200, 800, "Fractal", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    // create our geometries
    GLuint vbo, vao, ebo, tex;
    init_buffers(vbo, vao, ebo, tex);

    // init shader
    shader_t shader("simple-shader.vs", "simple-shader.fs");

    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    auto const start_time = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Get windows size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Set viewport to fill the whole window area
        glViewport(0, 0, display_w, display_h);

        // Fill background with solid color
        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float c[] = {-0.74543f, 0.11301f};
        static float shift1[] = {0.f, 0.f};
        static float shift2[] = {0.f, 0.f};
        static int n = 300;
        static float zoom = 1.0f;
        static float radius = (1.0f + sqrt(1.0f + 4.0f * sqrt(c[0] * c[0] + c[1] * c[1]))) / 2.0f;

        // GUI
        ImGui::Begin("Settings");
        ImGui::Text("Fractal settings");
        ImGui::SliderFloat("centre.x", c, -1, 1);
        ImGui::SliderFloat("centre.y", c + 1, -1, 1);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Quality settings");

        ImGui::SliderInt("Iterations", &n, 1, 800);
        ImGui::SliderFloat("Radius", &radius, 0, 10);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Color settings");

        ImGui::Columns(NT, nullptr, false);
        for (int i = 0; i < NT; ++i) {
            auto name = std::to_string(i);
            ImGui::ColorEdit3(name.c_str(), T + (3 * i), 1 << 5 | 1 << 7);
            ImGui::NextColumn();
        }
        ImGui::Columns(1);

        ImGui::End();

        int width, height;
        glfwGetWindowSize(window, &width, &height); // since ImGui::GetWindowHeight() returns width
	float screen_ratio = static_cast<float>(width) / height;

        float wheel = ImGui::GetIO().MouseWheel;
        if (abs(wheel) > 0.1) {
            auto mouse = ImGui::GetMousePos();

            float x = mouse.x / static_cast<float>(width) * 2 - 1;
            float y = -(mouse.y / static_cast<float>(height) * 2 - 1);
            wheel = wheel > 0 ? 1/1.2f : 1.2f;
           
            mouse.x /= screen_ratio;

	    shift2[0] += (x + shift1[0]) * (1 - wheel) * zoom;
            shift2[1] += (y + shift1[1]) * (1 - wheel) * zoom;

            zoom *= wheel;
        }

        if (!ImGui::IsAnyWindowFocused()) {
            auto delta = ImGui::GetMouseDragDelta(0, 0);
            ImGui::ResetMouseDragDelta();
            shift1[0] -= delta.x / static_cast<float>(width) * 2;
            shift1[1] += delta.y / static_cast<float>(height) * 2;
        }

        // Pass the parameters to the shader as uniforms
        shader.set_uniform("u_c", c[0], c[1]);
        shader.set_uniform("u_n", n);
        shader.set_uniform("u_zoom", zoom);
        shader.set_uniform("u_radius", radius);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, NT, 0, GL_RGB, GL_FLOAT, T);
        shader.set_uniform("u_texture", 0);
        shader.set_uniform("scale", static_cast<float>(width)/height, 1.0f);

        shader.set_uniform("u_shift1", shift1[0], shift1[1]);
        shader.set_uniform("u_shift2", shift2[0], shift2[1]);

        // Bind triangle shader
        shader.use();
        // Bind vertex array = buffers + indices
        glBindVertexArray(vao);

        // Execute draw call
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        // Generate gui render commands
        ImGui::Render();

        // Execute gui render commands using OpenGL backend
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the backbuffer with the frontbuffer that is used for screen display
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
