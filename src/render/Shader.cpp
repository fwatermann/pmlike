//
// Created by finnw on 28.01.2022.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.hpp"

using namespace pmlike::render;

Shader::Shader(const std::string& source, GLenum type, bool isFile) {
    this->source = source;
    this->type = type;
    this->isFile = isFile;
}

Shader::Shader(const Shader& other) {
    this->source = other.source;
    this->type = other.type;
    this->isFile = other.isFile;
    this->shader = other.shader;
}

Shader::~Shader() {
    this->del();
}

void Shader::del() {
    if(!this->shader) return;
    glDeleteShader(this->shader);
    this->shader = 0;
}

bool Shader::compile() {

    if(this->isFile) {
        std::fstream file;
        file.open(this->source, std::ios::in);
        if(!file.is_open()) {
            std::printf("Shader file not found: %s\n", this->source.c_str());
            return false;
        } else {
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();
            this->source = stream.str();
        }
    }

    this->shader = glCreateShader(this->type);
    const char* sourceCStr = this->source.c_str();
    glShaderSource(this->shader, 1, &sourceCStr, nullptr);
    glCompileShader(this->shader);
    GLint compiled = 0;
    glGetShaderiv(this->shader, GL_COMPILE_STATUS, &compiled);
    if(compiled == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(this->shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, ' ');
        glGetShaderInfoLog(this->shader, length, &length, &log[0]);
        std::printf("Shader compilation failed: %s\n", log.c_str());
        glDeleteShader(this->shader);
        this->shader = 0;
        return false;
    }
    this->compiled = true;
    return true;
}

GLuint Shader::getHandle() const {
    return this->shader;
}