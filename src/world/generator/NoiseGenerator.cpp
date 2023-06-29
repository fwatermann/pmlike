//
// Created by finnw on 31.05.2023.
//

#include "NoiseGenerator.hpp"
#include "glm/gtc/noise.hpp"
#include "util/Log.hpp"

using namespace pmlike::world::generator;

NoiseGenerator::NoiseGenerator() {

}

NoiseGenerator::~NoiseGenerator() {

}

void NoiseGenerator::generate(std::shared_ptr<world::Chunk> chunk) {
    glm::ivec3 chunkCoords = chunk->getChunkCoordinates();
    glm::ivec3 worldCoords = chunkCoords * glm::ivec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);

    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                glm::ivec3 blockCoords = {worldCoords.x + x, worldCoords.y + y, worldCoords.z + z};
                float noise = glm::perlin(glm::vec2(blockCoords.x, blockCoords.z) / 250.0f) * 64.0f + 64.0f;
                if (noise > blockCoords.y) {
                    chunk->blockAt(glm::ivec3(x, y, z), world::material::block::BlockMaterial::STONE);
                    continue;
                }
                chunk->blockAt(glm::ivec3(x, y, z), world::material::block::BlockMaterial::AIR);
            }
        }
    }

    chunk->updateMesh(true);
    chunk->generated = true;
}
