//
// Created by finnw on 29.01.2022.
//

#include <string>
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "ShaderProgram.hpp"
#include "util/Log.hpp"

using namespace pmlike::render;

ShaderProgram::ShaderProgram() {
    throw std::runtime_error("ShaderProgram::ShaderProgram() - Could not create ShaderProgram without any shaders.");
}

ShaderProgram::ShaderProgram(const ShaderProgram &other) {
    this->program = other.program;
    this->uniforms = other.uniforms;
}

ShaderProgram::ShaderProgram(Shader *shader1, Shader *shader2, Shader *shader3) {
    if (shader3 != nullptr) {
        Shader *shaders[] = {shader1, shader2, shader3};
        loadShaders(&shaders[0], 3);
    } else {
        Shader *shaders[] = {shader1, shader2};
        loadShaders(&shaders[0], 2);
    }
}


ShaderProgram::ShaderProgram(Shader **shaders, std::size_t count) {
    this->loadShaders(shaders, count);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(program);
}

bool ShaderProgram::loadShaders(Shader **shaders, std::size_t count) {
    this->program = glCreateProgram();

    //Compile all shaders
    for (size_t i = 0; i < count; i++) {
        Shader *shader = shaders[i];
        shader->compile();
    }

    //Attach all shaders to program
    for (uint8_t i = 0; i < count; i++) {
        Shader *shader = shaders[i];
        glAttachShader(this->program, shader->getHandle());
    }

    //Link ShaderProgram
    glLinkProgram(this->program);

    GLint success;
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, ' ');
        glGetProgramInfoLog(this->program, length, &length, &log[0]);
        LOG_WF("Failed to link shader program: \n%s", log.c_str());
        glDeleteProgram(this->program);
        this->program = 0;
        return false;
    }

    //Detach all shaders
    for (uint8_t i = 0; i < count; i++) {
        Shader *shader = shaders[i];
        glDetachShader(this->program, shader->getHandle());
    }

    //Delete all shaders
    for (uint8_t i = 0; i < count; i++) {
        Shader *shader = shaders[i];
        shader->del();
    }
    this->loadUniforms();
    LOG_D("Shader program successfully linked.");
    return true;
}

void ShaderProgram::use() const {
    glUseProgram(this->program);
}

void ShaderProgram::unuse() const {
    glUseProgram(0);
}

GLint ShaderProgram::getAttribLocation(const std::string &name) {
    return glGetAttribLocation(this->program, name.c_str());
}

void ShaderProgram::loadUniforms() {
    GLint uniformCount;
    glGetProgramiv(this->program, GL_ACTIVE_UNIFORMS, &uniformCount);
    LOG_DF("Found %d uniforms.", uniformCount);
    for (int i = 0; i < uniformCount; i++) {
        GLint size;
        GLenum type;
        GLchar name[256];
        glGetActiveUniform(this->program, i, 256, nullptr, &size, &type, name);
        GLint location = glGetUniformLocation(this->program, name);
        if (location == -1) continue;
        this->uniforms.insert(std::pair<std::string, GLint>(name, location));
        LOG_DF("Found Uniform '%s'@%d", name, location);
    }
}

/*
 * Set the value of a uniform variable in the shader program.
 */

void ShaderProgram::setUniform1i(const std::string &name, int value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform1i(this->uniforms[name], value);
    }
}

void ShaderProgram::setUniform1f(const std::string &name, float value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform1f(this->uniforms[name], value);
    }
}

/*
 * Set uniform value for a vec2
 */

void ShaderProgram::setUniform2f(const std::string &name, float value1, float value2) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform2f(this->uniforms[name], value1, value2);
    }
}

void ShaderProgram::setUniform2i(const std::string &name, int value1, int value2) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform2i(this->uniforms[name], value1, value2);
    }
}

void ShaderProgram::setUniform2(const std::string &name, glm::vec2 value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform2f(this->uniforms[name], value.x, value.y);
    }
}

/*
 * Set uniform value for a vec3
 */

void ShaderProgram::setUniform3f(const std::string &name, float value1, float value2, float value3) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform3f(this->uniforms[name], value1, value2, value3);
    }
}

void ShaderProgram::setUniform3i(const std::string &name, int value1, int value2, int value3) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform3i(this->uniforms[name], value1, value2, value3);
    }
}

void ShaderProgram::setUniform3(const std::string &name, glm::vec3 value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform3f(this->uniforms[name], value.x, value.y, value.z);
    }
}

/*
 * Set uniform value for a vec4
 */

void ShaderProgram::setUniform4f(const std::string &name, float value1, float value2, float value3, float value4) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform4f(this->uniforms[name], value1, value2, value3, value4);
    }
}

void ShaderProgram::setUniform4i(const std::string &name, int value1, int value2, int value3, int value4) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform4i(this->uniforms[name], value1, value2, value3, value4);
    }
}

void ShaderProgram::setUniform4(const std::string &name, glm::vec4 value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniform4f(this->uniforms[name], value.x, value.y, value.z, value.w);
    }
}

/**
 * Set uniform value for various matrices
 */

void ShaderProgram::setUniformMat2(const std::string &name, glm::mat2 value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniformMatrix2fv(this->uniforms[name], 1, GL_FALSE, &value[0][0]);
    }
}

void ShaderProgram::setUniformMat3(const std::string &name, glm::mat3 value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniformMatrix3fv(this->uniforms[name], 1, GL_FALSE, &value[0][0]);
    }
}

void ShaderProgram::setUniformMat4(const std::string &name, glm::mat4 value) {
    if (this->uniforms.find(name) == this->uniforms.end()) {
        LOG_WF("Uniform '%s' not found!", name.c_str());
        return;
    } else {
        glUniformMatrix4fv(this->uniforms[name], 1, GL_FALSE, &value[0][0]);
    }
}






