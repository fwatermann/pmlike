//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_BLOCK_HPP
#define PM_LIKE_BLOCK_HPP

#include <memory>
#include "glm/glm.hpp"

namespace pmlike::world::block {

    enum BlockMaterial {
        AIR,
        STONE
    };

    class Block {
        public:
            BlockMaterial material;

            Block(BlockMaterial material, glm::ivec3 worldCoordinates, glm::ivec3 chunkCoordinates);

            ~Block();

            glm::ivec3 getWorldCoordinate();

            glm::ivec3 getChunkCoordinate();

            bool isTransparent();

        private:
            glm::ivec3 worldCoordinate{};
            glm::ivec3 chunkCoordinate{};

    };

}

#endif //PM_LIKE_BLOCK_HPP
