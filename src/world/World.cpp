//
// Created by finnw on 31.05.2023.
//

#include <random>
#include <algorithm>
#include <utility>
#include "World.hpp"
#include "util/Log.hpp"

using namespace pmlike::world;

std::shared_ptr<World> World::instance = nullptr;
std::shared_ptr<World> World::getInstance() {
    if(instance == nullptr) {
        instance = std::shared_ptr<World>(new World());
    }
    return instance;
}

void World::deleteInstance() {
    if(World::instance == nullptr) return;
    //delete World::instance.get(); TODO: Check if this is necessary
    World::instance = nullptr;
}

World::World() {
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                this->chunks[x][y][z] = std::vector<std::shared_ptr<Chunk>>();
            }
        }
    }
}

World::~World() = default;

void World::render(std::shared_ptr<render::Camera> camera, double deltaTime) {

    bool depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    bool cullFaceEnabled = glIsEnabled(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    this->loadedChunksLock.lock();
    this->renderedChunks = 0;
    this->currentCamera = std::move(camera);
    for (const std::shared_ptr<Chunk>& chunk: this->loadedChunks) {
        if (chunk == nullptr) continue;
        glm::vec3 minP = glm::vec3(chunk->getChunkCoordinates().x * CHUNK_SIZE_X,
                                   chunk->getChunkCoordinates().y * CHUNK_SIZE_Y,
                                   chunk->getChunkCoordinates().z * CHUNK_SIZE_Z);
        glm::vec3 maxP = minP + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
        if (this->currentCamera->frustum.IsBoxVisible(minP, maxP)) {
            chunk->render(currentCamera, deltaTime);
            this->renderedChunks++;
        }
    }
    for(const std::shared_ptr<Chunk> &chunk : this->unloadChunks) {
        if(chunk == nullptr) continue;
        chunk->unloadMesh();
    }
    this->unloadChunks.clear();
    this->unloadChunks.shrink_to_fit();

    this->loadedChunksLock.unlock();

    if (!depthEnabled) glDisable(GL_DEPTH_TEST);
    if (!cullFaceEnabled) glDisable(GL_CULL_FACE);

}

std::shared_ptr<Chunk> World::getChunk(glm::ivec3 chunkCoordinates) {
    int a = std::abs(chunkCoordinates.x % CHUNK_TREE_BUCKET_SIZE);
    int b = std::abs(chunkCoordinates.y % CHUNK_TREE_BUCKET_SIZE);
    int c = std::abs(chunkCoordinates.z % CHUNK_TREE_BUCKET_SIZE);
    std::vector<std::shared_ptr<Chunk>> &list = this->chunks[a][b][c];
    for (std::shared_ptr<Chunk> chunk: list) {
        if (chunk == nullptr) continue;
        if (chunk->getChunkCoordinates().x == chunkCoordinates.x &&
            chunk->getChunkCoordinates().y == chunkCoordinates.y &&
            chunk->getChunkCoordinates().z == chunkCoordinates.z) {
            return chunk;
        }
    }
    return nullptr;
}

void World::putChunk(const std::shared_ptr<Chunk> &chunk) {
    glm::ivec3 chunkCoordinates = chunk->getChunkCoordinates();
    //LOG_DF("Generation Chunk at %d %d %d", chunkCoordinates.x, chunkCoordinates.y, chunkCoordinates.z);
    int a = std::abs(chunkCoordinates.x % CHUNK_TREE_BUCKET_SIZE);
    int b = std::abs(chunkCoordinates.y % CHUNK_TREE_BUCKET_SIZE);
    int c = std::abs(chunkCoordinates.z % CHUNK_TREE_BUCKET_SIZE);
    std::vector<std::shared_ptr<Chunk>> &list = this->chunks[a][b][c];
    std::mutex &mutex = this->chunksLock[a][b][c];
    mutex.lock();
    auto result = std::find_if(list.begin(), list.end(), [chunkCoordinates](std::shared_ptr<Chunk> chunk) {
        return chunk->getChunkCoordinates().x == chunkCoordinates.x &&
               chunk->getChunkCoordinates().y == chunkCoordinates.y &&
               chunk->getChunkCoordinates().z == chunkCoordinates.z;
    });
    if (result != list.end()) {
        list.erase(result);
    }
    list.push_back(chunk);
    mutex.unlock();
}

void World::removeChunk(const std::shared_ptr<Chunk> &chunk) {
    glm::ivec3 chunkCoordinates = chunk->getChunkCoordinates();
    int a = std::abs(chunkCoordinates.x % CHUNK_TREE_BUCKET_SIZE);
    int b = std::abs(chunkCoordinates.y % CHUNK_TREE_BUCKET_SIZE);
    int c = std::abs(chunkCoordinates.z % CHUNK_TREE_BUCKET_SIZE);
    std::vector<std::shared_ptr<Chunk>> &list = this->chunks[a][b][c];
    std::mutex &mutex = this->chunksLock[a][b][c];
    mutex.lock();
    auto result = std::find_if(list.begin(), list.end(), [chunkCoordinates](std::shared_ptr<Chunk> chunk) {
        return chunk->getChunkCoordinates().x == chunkCoordinates.x &&
               chunk->getChunkCoordinates().y == chunkCoordinates.y &&
               chunk->getChunkCoordinates().z == chunkCoordinates.z;
    });
    if (result != list.end()) {
        list.erase(result);
    }
    list.shrink_to_fit();
    mutex.unlock();
}

void World::setLoadDistance(glm::ivec3 d) {
    this->loadDistance = d;
}

glm::ivec3 World::getLoadDistance() {
    return {this->loadDistance};
}

void World::setGenerator(std::shared_ptr<ChunkGenerator> chunkGenerator) {
    this->generator = std::move(chunkGenerator);
}


