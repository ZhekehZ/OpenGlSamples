#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_utils/opengl_shader.h"
#include "file_utils.h"
#include "buffer_utils/texture_loader.h"
#include "buffer_utils/cube_data.h"
#include "buffer_utils/init_ogl_buffer.h"


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
            textures.get_s("sb_right.jpg"),
            textures.get_s("sb_left.jpg"),
            textures.get_s("sb_up.jpg"),
            textures.get_s("sb_down.jpg"),
            textures.get_s("sb_front.jpg"),
            textures.get_s("sb_back.jpg")
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

};