#pragma once

#include <vector>

struct MeshPart {
    GLuint vao = 0;
    GLuint texture{};
    int size = 0;

    bool with_texture = false;
};

struct Mesh {
    std::vector<MeshPart> parts;

    void draw(shader_t & shader) {
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

            glBindVertexArray(0);
        }
    }
};