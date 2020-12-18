#pragma once

#include <string>
#include <vector>
#include <tuple>

#include <GL/glew.h>
#include <stdexcept>

class shader_t {
public:
    shader_t(
        const std::string& vertex_code_fname,
        const std::string& fragment_code_fname,
        const std::string& geometry_code_fname
    );
    ~shader_t();

    void use() const;
    template<typename T>
    void set_uniform(const std::string& name, T val);
    template<typename T>
    void set_uniform(const std::string& name, T val1, T val2);
    template<typename T>
    void set_uniform(const std::string& name, T val1, T val2, T val3);

private:
    void check_compile_error() const;
    void check_linking_error() const;
    void compile(
        const std::string& vertex_code,
        const std::string& fragment_code,
        const std::string& geometry_code
    );
    void link();

    GLuint vertex_id_;
    GLuint fragment_id_;
    GLuint geometry_id_;
    GLuint program_id_;

    bool without_geom;
};
