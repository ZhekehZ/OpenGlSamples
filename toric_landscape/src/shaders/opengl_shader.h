#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <GL/glew.h>

using namespace std::filesystem;

class shader_t {
public:
    shader_t() = default;
    explicit shader_t(path);
    ~shader_t();

    void use() const;

    template<typename T>
    [[maybe_unused]] void set_uniform(const std::string &name, T val);
    template<typename T>
    [[maybe_unused]] [[maybe_unused]] [[maybe_unused]] void set_uniform(const std::string &name, T val1, T val2);
    template<typename T>
    [[maybe_unused]] [[maybe_unused]] void set_uniform(const std::string &name, T val1, T val2, T val3);

private:
    void check_compile_error() const;
    void check_linking_error() const;
    void compile(const std::string &vertex_code, const std::string &fragment_code);
    void link();

    GLuint vertex_id_{}, fragment_id_{}, program_id_{};
};
