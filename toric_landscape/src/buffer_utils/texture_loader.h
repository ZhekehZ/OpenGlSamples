#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint load_single_texture(std::string const &name) {
    GLuint texture;

    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char *image = stbi_load(name.c_str(), &width, &height, &channels, STBI_rgb);

    if (!image) std::cerr << "ERROR LOADING IMAGE " << name << std::endl;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    return texture;
}

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