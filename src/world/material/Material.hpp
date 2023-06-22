//
// Created by finnw on 22.06.2023.
//

#ifndef PM_LIKE_MATERIAL_HPP
#define PM_LIKE_MATERIAL_HPP

namespace pmlike::world::material::block {

    enum BlockMaterial {
        AIR,
        STONE
    };

    bool isTransparent(BlockMaterial material);
    bool isTransparent(BlockMaterial* material);

}


#endif //PM_LIKE_MATERIAL_HPP
