//
// Created by finnw on 31.05.2023.
//

#include "DungeonGenerator.hpp"
#include "util/Log.hpp"


using namespace pmlike::world::generator;

DungeonGenerator::DungeonGenerator(int seed) : seed(seed) {
    this->noise = FastNoise::New<FastNoise::Perlin>();
}

DungeonGenerator::DungeonGenerator() {
    this->noise = FastNoise::New<FastNoise::Perlin>();
    this->seed = std::chrono::system_clock::now().time_since_epoch().count();
}

DungeonGenerator::~DungeonGenerator() {

}

void DungeonGenerator::generate(std::shared_ptr<world::Chunk> chunk) {
    glm::ivec3 chunkCoords = chunk->getChunkCoordinates();
    glm::ivec3 worldCoords = chunkCoords * glm::ivec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);


    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                glm::ivec3 blockCoords = {worldCoords.x + x, worldCoords.y + y, worldCoords.z + z};
                float noise = this->noise->GenSingle3D(blockCoords.x / 100.0f, blockCoords.y / 100.0f, blockCoords.z / 100.0f, this->seed);

                if (noise > 0.0f) {
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

int DungeonGenerator::getSeed() {
    return this->seed;
}
