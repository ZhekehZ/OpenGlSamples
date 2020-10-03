#pragma once

#include <GL/glew.h>
#include <vector>

struct MeshPart {
    GLuint vao = 0;
    GLuint texture{};
    int size = 0;

    bool with_texture = false;
};

struct Mesh {
    std::vector<MeshPart> parts;

    void draw(shader_t & shader, bool pre_test) {
        if (pre_test) {
            start_Z_buffer_pre_pass();
            for (const auto &part : parts) {
                glBindVertexArray(part.vao);
                glDrawArrays(GL_TRIANGLES, 0, part.size);
            }
            end_Z_buffer_pre_pass();
        }

        for (const auto & part : parts) {
            if (part.with_texture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, part.texture);
                shader.set_uniform("u_texture", 0);
                shader.set_uniform("is_textured", 1.0f);
            } else {
                shader.set_uniform("is_textured", 0.0f);
            }

            glBindVertexArray(part.vao);
            glDrawArrays(GL_TRIANGLES, 0, part.size);
        }

        glDepthMask(GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

private:
    static void start_Z_buffer_pre_pass() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glColorMask(0, 0, 0, 0);
        glDepthMask(GL_TRUE);
    }

    static void end_Z_buffer_pre_pass() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glColorMask(1, 1, 1, 1);
        glDepthMask(GL_FALSE);
    }
};