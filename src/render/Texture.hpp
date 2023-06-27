//
// Created by finnw on 29.01.2022.
//

#ifndef CRAFTVENTURE_TEXTURE_H
#define CRAFTVENTURE_TEXTURE_H

#include <string>
#include "ShaderProgram.hpp"

namespace pmlike::render {

    class Texture {
        protected:

        public:
            ~Texture();

            static Texture *fromFile(const std::string &path);

            static Texture *fromData(uint8_t *data, int width, int height, int channels);

            static Texture *empty(int width, int height);

            void bind(GLuint slot = 0);

            void bindToProgram(GLuint slot, ShaderProgram *program, const std::string &name);

            void unbind();

            GLuint getHandle() const;

            void setMagFilter(GLint filter);

            void setMinFilter(GLint filter);

        private:
            Texture(uint8_t *data, int width, int height, int channels);

            GLuint handle{};
    };


}


#endif //CRAFTVENTURE_TEXTURE_H
