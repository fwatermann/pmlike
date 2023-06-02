//
// Created by finnw on 28.01.2022.
//

#ifndef CRAFTVENTURE_SHADER_H
#define CRAFTVENTURE_SHADER_H

#include <string>
#include "glad/glad.h"

namespace pmlike::render {

    class Shader {
        public:
            bool compiled = false;
            std::string source;

            Shader(const std::string& source, GLenum type, bool isFile = false);
            Shader(const Shader& other); // copy constructor
            ~Shader();
            bool compile();
            GLuint getHandle() const;
            void del();
        private:
            GLuint shader;
            GLenum type;
            bool isFile;

    };

}

#endif //CRAFTVENTURE_SHADER_H