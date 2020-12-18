#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <GL/glew.h>
#include <stb_image.h>

GLuint load_cube_texture(std::array<std::string, 6> const &names) {
    GLuint texture;
    int width, height, channels;

    stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    unsigned idx = 0;
    for (auto const &name : names) {
        unsigned char *image = stbi_load(name.c_str(), &width, &height, &channels, 0);
        if (image) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx++, 0, GL_RGB, width, height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else {
            std::cerr << "ERROR LOADING IMAGE " << name << std::endl;
        }
        stbi_image_free(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}
