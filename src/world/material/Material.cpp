//
// Created by finnw on 22.06.2023.
//

#include "Material.hpp"

using namespace pmlike::world::material;

bool block::isTransparent(BlockMaterial material) {
    return material == BlockMaterial::AIR;
    //TODO: Check for further transparent materials if needed/added
}

bool block::isTransparent(BlockMaterial* material) {
    return block::isTransparent(*material);
}

glm::vec2 block::getTextureIndex(BlockMaterial material) {
    switch (material) {
        case BlockMaterial::AIR:
            return {-1, -1};
        case BlockMaterial::STONE:
            return {0, 0};
        case BlockMaterial::GRASS_BLOCK:
            return {1, 0};
        default:
            return {0, 0};
    }
}

