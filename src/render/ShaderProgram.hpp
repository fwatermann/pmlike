//
// Created by finnw on 29.01.2022.
//

#ifndef CRAFTVENTURE_SHADERPROGRAM_H
#define CRAFTVENTURE_SHADERPROGRAM_H

#include <string>
#include <map>
#include <vector>
#include "glm/glm.hpp"
#include "Shader.hpp"

namespace pmlike::render {

    class ShaderProgram {
        public:
            ShaderProgram();
            ~ShaderProgram();
            ShaderProgram(Shader** shaders, std::size_t count);
            ShaderProgram(Shader* shader1, Shader* shader2, Shader* shader3 = nullptr);
            ShaderProgram(const ShaderProgram& other); // copy constructor

            /**
             * Links a Shader Program
             * @param shaders Pointer to the shaders that should be combined to one shader program.
             * @param count Count of shaders that will be combined.
             * @return true on success else false
             */
            bool loadShaders(Shader** shaders, std::size_t count);

            /**
             * Uses the Shader Program
             */
            void use() const;

            /**
             * Unbinds the Shader Program
             */
            void unuse() const;

            /**
             * Get the attribute Location of a specific attribute
             *
             * @param name
             * @return GLuint
             */
            GLint getAttribLocation(const std::string &name);

            /**
             * Sets a Uniform Variable
             * @param name Name of the Uniform Variable
             * @param value Value of the Uniform Variable
             */
            void setUniform1i(const std::string& name, int value);

            /**
             * Sets a Uniform Variable
             * @param name Name of the Uniform Variable
             * @param value Value of the Uniform Variable
             */
            void setUniform1f(const std::string& name, float value);

            /**
             * Sets a Uniform 2D Vector
             * @param name Name of the Uniform Variable
             * @param value X Value of the Uniform Variable
             * @param value Y Value of the Uniform Variable
             */
            void setUniform2f(const std::string& name, float x, float y);

            /**
             * Sets a Uniform 2D Vector
             * @param name Name of the Uniform Variable
             * @param value X Value of the Uniform Variable
             * @param value Y Value of the Uniform Variable
             */
            void setUniform2i(const std::string& name, int x, int y);

            /**
             * Sets a Uniform 2D Vector
             * @param name Name of the Uniform Variable
             * @param value Vector to set
             */
            void setUniform2(const std::string&, glm::vec2 value);

            /**
             * Sets a Uniform 3D Vector
             * @param name Name of the Uniform Variable
             * @param value Vector to set
             */
            void setUniform3(const std::string& name, glm::vec3 value);

            /**
             * Sets a Uniform 3D Vector
             * @param name Name of the Uniform Variable
             * @param x X Value of the Uniform Variable
             * @param y Y Value of the Uniform Variable
             * @param z Z Value of the Uniform Variable
             */
            void setUniform3f(const std::string& name, float x, float y, float z);

            /**
             * Sets a Uniform 3D Vector
             * @param name Name of the Uniform Variable
             * @param x X Value of the Uniform Variable
             * @param y Y Value of the Uniform Variable
             * @param z Z Value of the Uniform Variable
             */
            void setUniform3i(const std::string& name, int x, int y, int z);

            /**
             * Sets a Uniform 4D Vector
             * @param name Name of the Uniform Variable
             * @param value Vector to set
             */
            void setUniform4(const std::string& name, glm::vec4 value);

            /**
             * Sets a Uniform 4D Vector
             * @param name Name of the Uniform Variable
             * @param x X Value of the Uniform Variable
             * @param y Y Value of the Uniform Variable
             * @param z Z Value of the Uniform Variable
             * @param w W Value of the Uniform Variable
             */
            void setUniform4f(const std::string& name, float x, float y, float z, float w);

            /**
             * Sets a Uniform 4D Vector
             * @param name Name of the Uniform Variable
             * @param x X Value of the Uniform Variable
             * @param y Y Value of the Uniform Variable
             * @param z Z Value of the Uniform Variable
             * @param w W Value of the Uniform Variable
             */
            void setUniform4i(const std::string& name, int x, int y, int z, int w);

            /**
             * Sets a Uniform 2x2 Matrix
             * @param name Name of the Uniform Variable
             * @param value 2x2 Matrix to set
             */
            void setUniformMat2(const std::string& name, glm::mat2 value);

            /**
             * Sets a Uniform 3x3 Matrix
             * @param name Name of the Uniform Variable
             * @param value 3x3 Matrix to set
             */
            void setUniformMat3(const std::string& name, glm::mat3 value);

            /**
             * Sets a Uniform 4x4 Matrix
             * @param name Name of the Uniform Variable
             * @param value 4x4 Matrix to set
             */
            void setUniformMat4(const std::string& name, glm::mat4 value);

            GLuint getGLHandle() const {
                return this->program;
            }

        private:
            GLuint program = 0;
            void loadUniforms();
            std::map<std::string, int> uniforms;
    };

}

#endif //CRAFTVENTURE_SHADERPROGRAM_H