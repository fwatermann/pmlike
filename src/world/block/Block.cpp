//
// Created by finnw on 31.05.2023.
//

#include "Block.hpp"

#include <utility>
#include "../Chunk.hpp"

using namespace pmlike::world::block;

Block::Block(BlockMaterial material, glm::ivec3 worldCoordinates, glm::ivec3 chunkCoords) : material(material) {
    this->worldCoordinate = worldCoordinates;
    this->chunkCoordinate = chunkCoords;

    /*int chunkX = 0, chunkY = 0, chunkZ = 0;
    if(worldCoordinates.x >= 0) {
        chunkX = worldCoordinates.x % CHUNK_SIZE;
    } else {
        chunkX = CHUNK_SIZE - (glm::abs(worldCoordinates.x) % CHUNK_SIZE);
    }
    if(worldCoordinates.y >= 0) {
        chunkY = worldCoordinates.y % CHUNK_SIZE;
    } else {
        chunkY = CHUNK_SIZE - (glm::abs(worldCoordinates.y) % CHUNK_SIZE);
    }
    if(worldCoordinates.z >= 0) {
        chunkZ = worldCoordinates.z % CHUNK_SIZE;
    } else {
        chunkZ = CHUNK_SIZE - (glm::abs(worldCoordinates.z) % CHUNK_SIZE);
    }
    this->chunkCoordinate = {chunkX, chunkY, chunkZ};
     */
}

Block::~Block() = default;

glm::ivec3 Block::getWorldCoordinate() {
    return {this->worldCoordinate};
}

glm::ivec3 Block::getChunkCoordinate() {
    return {this->chunkCoordinate};
}

bool Block::isTransparent() {
    return this->material == BlockMaterial::AIR;
}
