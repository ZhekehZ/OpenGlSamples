#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/shaders/opengl_shader.h>
#include <src/io/texture_loader.h>
#include <src/io/cube_data.h>

using namespace std::filesystem;

template <int TextureSlot>
class Skybox {
    shader_t shader_;
    GLuint texture_;
    GLuint vertex_data_;

public:
    Skybox(Resource const & textures, const shader_t & shader)
        : shader_(shader)
    {
        texture_ = load_cube_texture({
            textures.get_s("right.png"),
            textures.get_s("left.png"),
            textures.get_s("up.png"),
            textures.get_s("down.png"),
            textures.get_s("front.png"),
            textures.get_s("back.png")
       });
        vertex_data_ = init_ogl_buffer(cubeV, cubeVSize, cubeF, cubeFSize);

        glActiveTexture(GL_TEXTURE0 + TextureSlot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    }

    template <bool SimpleShader = false>
    void draw(glm::mat4 & view_projection) {
        if (!SimpleShader) {
            shader_.use();
            shader_.set_uniform("u_skybox", TextureSlot);
            shader_.set_uniform("u_mvp", glm::value_ptr(view_projection));
        }

        glBindVertexArray(vertex_data_);
        glDrawElements(GL_TRIANGLES, cubeFSize, GL_UNSIGNED_INT, nullptr);
    }

private:
    static GLuint init_ogl_buffer(float *v_data, int v_size, int *i_data, int i_size) {
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
};