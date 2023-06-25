//
// Created by finnw on 22.06.2023.
//

#ifndef PM_LIKE_MATERIAL_HPP
#define PM_LIKE_MATERIAL_HPP

#include "glm/glm.hpp"

namespace pmlike::world::material::block {

    enum BlockMaterial {
        AIR,
        STONE,
        GRASS_BLOCK,
    };

    bool isTransparent(BlockMaterial material);
    bool isTransparent(BlockMaterial* material);

    glm::vec2 getTextureIndex(BlockMaterial material);


}


#endif //PM_LIKE_MATERIAL_HPP
