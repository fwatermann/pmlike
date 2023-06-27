//
// Created by finnw on 25.06.2023.
//

#ifndef PM_LIKE_TEXTUREATLAS_H
#define PM_LIKE_TEXTUREATLAS_H

#include <memory>
#include "Texture.hpp"

namespace pmlike::render {
    class TextureAtlas {

    public:
            TextureAtlas(std::vector<std::string> textures);

            std::shared_ptr<Texture> getTexture();

            void init();

    private:
        std::shared_ptr<Texture> texture;
        std::vector<std::string> texturePaths;
    };
}




#endif //PM_LIKE_TEXTUREATLAS_H
