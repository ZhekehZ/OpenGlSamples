#pragma once

#include <iostream>
#include <GL/gl.h>
#include <fmt/format.h>

inline void GLAPIENTRY MessageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam
) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cerr << "---------------" << std::endl;
    std::cerr << "Debug message (" << id << "): " << message << std::endl;
    std::cerr << "Source: ";
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cerr << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cerr << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cerr << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cerr << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cerr << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cerr << "Other";
            break;
    }
    std::cerr << std::endl << "Type: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cerr << "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cerr << "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cerr << "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cerr << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cerr << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cerr << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cerr << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cerr << "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cerr << "Other";
            break;
    }
    std::cerr << std::endl << "Severity: ";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cerr << "high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cerr << "medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cerr << "low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cerr << "notification";
            break;
    }
    std::cerr << std::endl;
    std::cerr << "---------------" << std::endl;
}

inline void glfw_error_callback(int error, const char *description) {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}
