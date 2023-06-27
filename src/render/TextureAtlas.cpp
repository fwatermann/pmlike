//
// Created by finnw on 25.06.2023.
//

#include "TextureAtlas.h"

#include <utility>

using namespace pmlike::render;

TextureAtlas::TextureAtlas(std::vector<std::string> textures) : texturePaths(std::move(textures)) {


}

std::shared_ptr<Texture> TextureAtlas::getTexture() {
    return texture;
}

void TextureAtlas::init() {

    

    for(std::string& path : this->texturePaths) {
        Texture* texture = Texture::fromFile(path);
        texture->setMagFilter(GL_NEAREST);
        texture->setMinFilter(GL_NEAREST);



    }

}
