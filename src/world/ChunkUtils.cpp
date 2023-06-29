//
// Created by finnw on 29.06.2023.
//

#include "ChunkUtils.hpp"

using namespace pmlike::world;

std::vector<std::shared_ptr<Chunk>> ChunkUtils::getNeighbourChunk(Chunk* chunk) {
    std::vector<std::shared_ptr<Chunk>> neighbours;
    if(chunk == nullptr) {
        return neighbours;
    }
    glm::ivec3 coords = chunk->getChunkCoordinates();
    std::shared_ptr<Chunk> xPos = getChunkAt(coords + glm::ivec3(1, 0, 0));
    if(xPos != nullptr && !xPos->destroyed) {
        neighbours.push_back(xPos);
    }
    std::shared_ptr<Chunk> xNeg = getChunkAt(coords + glm::ivec3(-1, 0, 0));
    if(xNeg != nullptr && !xNeg->destroyed) {
        neighbours.push_back(xNeg);
    }
    std::shared_ptr<Chunk> yPos = getChunkAt(coords + glm::ivec3(0, 1, 0));
    if(yPos != nullptr && !yPos->destroyed) {
        neighbours.push_back(yPos);
    }
    std::shared_ptr<Chunk> yNeg = getChunkAt(coords + glm::ivec3(0, -1, 0));
    if(yNeg != nullptr && !yNeg->destroyed) {
        neighbours.push_back(yNeg);
    }
    std::shared_ptr<Chunk> zPos = getChunkAt(coords + glm::ivec3(0, 0, 1));
    if(zPos != nullptr && !zPos->destroyed) {
        neighbours.push_back(zPos);
    }
    std::shared_ptr<Chunk> zNeg = getChunkAt(coords + glm::ivec3(0, 0, -1));
    if(zNeg != nullptr && !zNeg->destroyed) {
        neighbours.push_back(zNeg);
    }
    return neighbours;
}

std::vector<std::shared_ptr<Chunk>> ChunkUtils::getNeighbourChunk(std::shared_ptr<Chunk>& chunk) {
    return getNeighbourChunk(chunk.get());
}

std::shared_ptr<Chunk> ChunkUtils::getChunkAt(glm::ivec3 chunkCoordinates) {
    return World::getInstance()->getChunk(chunkCoordinates);
}
