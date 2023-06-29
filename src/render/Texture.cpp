//
// Created by finnw on 29.01.2022.
//

#include "Util/Log.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "Texture.hpp"

using namespace pmlike::render;

Texture *Texture::fromFile(const std::string &path) {
    int width, height, nrChannels;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    Texture *ret = new Texture(data, width, height, nrChannels);
    stbi_image_free(data);
    return ret;
}

Texture *Texture::fromData(uint8_t *data, int width, int height, int channels) {
    return new Texture(data, width, height, channels);
}

Texture *Texture::Texture::empty(int width, int height) {
    return nullptr;
}

Texture::Texture(uint8_t *data, int width, int height, int channels) {
    glGenTextures(1, &this->handle);
    glBindTexture(GL_TEXTURE_2D, this->handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format = GL_RGBA;
    switch (channels) {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            LOG_EF("Invalid number of channels: %d", channels);
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->handle);
}

void Texture::bind(const GLuint slot) {
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, this->handle);
}

void Texture::bindToProgram(const GLuint slot, ShaderProgram *program, const std::string &name) {
    if (program != nullptr) {
        glActiveTexture(slot);
        glBindTexture(GL_TEXTURE_2D, this->handle);
        program->setUniform1i(name, slot - GL_TEXTURE0);
    }
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getHandle() const {
    return this->handle;
}

void Texture::setMagFilter(GLint filter) {
    glBindTexture(GL_TEXTURE_2D, this->handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setMinFilter(GLint filter) {
    glBindTexture(GL_TEXTURE_2D, this->handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);
}
