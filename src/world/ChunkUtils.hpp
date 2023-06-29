//
// Created by finnw on 29.06.2023.
//

#ifndef PM_LIKE_CHUNKUTILS_HPP
#define PM_LIKE_CHUNKUTILS_HPP

#include "Chunk.hpp"
#include "World.hpp"

namespace pmlike::world::ChunkUtils {

    std::vector<std::shared_ptr<world::Chunk>> getNeighbourChunk(Chunk* chunk);
    std::vector<std::shared_ptr<world::Chunk>> getNeighbourChunk(std::shared_ptr<Chunk>& chunk);
    std::shared_ptr<world::Chunk> getChunkAt(glm::ivec3 chunkCoordinates);


}

#endif //PM_LIKE_CHUNKUTILS_HPP
