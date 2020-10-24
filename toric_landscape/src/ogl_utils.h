#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "fmt/format.h"

inline void glfw_error_callback(int error, const char *description) {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

inline GLFWwindow *init_OGL(std::string const &caption, int width = 800, int height = 600) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, caption.c_str(), nullptr, nullptr);
    if (window == nullptr) return nullptr;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return nullptr;
    }

    return window;
}

inline void dispose_OGL(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

inline void print_OGL_errors() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << gluErrorString(err) << std::endl;
    }
}