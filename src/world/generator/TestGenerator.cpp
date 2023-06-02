//
// Created by finnw on 31.05.2023.
//

#include "TestGenerator.hpp"

using namespace pmlike::world::generator;

TestGenerator::TestGenerator() = default;

TestGenerator::~TestGenerator() = default;

void TestGenerator::generate(std::shared_ptr<world::Chunk> chunk) {

    glm::ivec3 chunkCoords = chunk->getChunkCoordinates();
    glm::ivec3 worldCoords = chunkCoords * glm::ivec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);

    if(chunkCoords.y < 0) {
        for(int x = 0; x < CHUNK_SIZE_X; x++) {
            for(int y = 0; y < CHUNK_SIZE_Y; y++) {
                for(int z = 0; z < CHUNK_SIZE_Z; z++) {
                    chunk->blocks[x][y][z] = new world::block::Block(world::block::BlockMaterial::STONE, {worldCoords.x + x, worldCoords.y + y, worldCoords.z + z}, glm::ivec3(x, y, z));
                }
            }
        }
    } else {
        for(int x = 0; x < CHUNK_SIZE_X; x++) {
            for(int y = 0; y < CHUNK_SIZE_Y; y++) {
                for(int z = 0; z < CHUNK_SIZE_Z; z++) {
                    chunk->blocks[x][y][z] = new world::block::Block(world::block::BlockMaterial::AIR, {worldCoords.x + x, worldCoords.y + y, worldCoords.z + z}, glm::ivec3(x, y, z));
                }
            }
        }
    }

}
