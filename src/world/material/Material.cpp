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

