//
// Created by finnw on 31.05.2023.
//

#include <random>
#include <algorithm>
#include "World.hpp"
#include "util/Log.hpp"

using namespace pmlike::world;

World::World(ChunkGenerator *generator) {
    this->generator = generator;

    for(int x = 0; x < 16; x++) {
        for(int y = 0; y < 16; y++) {
            for(int z = 0; z < 16; z++) {
                this->chunks[x][y][z] = std::vector<std::shared_ptr<Chunk>>();
            }
        }
    }

    this->threadChunkLoad = std::thread(World::chunkWorkerLoad, this);
    this->threadChunkGenerate = std::thread(World::chunkWorkerGenerate, this);

}

World::World() : World(new generator::TestGenerator()) {}

World::~World() {
    delete this->generator;
}

void World::render(pmlike::render::Camera* camera, double deltaTime) {

    bool depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    bool cullFaceEnabled = glIsEnabled(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_FRONT);

    //Get sourounding chunks
    this->currentCamera = camera;
    this->loadedChunksLock.lock();
    this->renderedChunks = 0;
    for(std::shared_ptr<Chunk> chunk : this->loadedChunks) {
        if(chunk == nullptr) continue;
        glm::vec3 minP = glm::vec3(chunk->getChunkCoordinates().x * CHUNK_SIZE_X, chunk->getChunkCoordinates().y * CHUNK_SIZE_Y, chunk->getChunkCoordinates().z * CHUNK_SIZE_Z);
        glm::vec3 maxP = minP + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
        if(camera->frustum.IsBoxVisible(minP, maxP)) {
            chunk->render(camera, deltaTime);
            this->renderedChunks++;
        }
    }
    this->loadedChunksLock.unlock();

    if(!depthEnabled) glDisable(GL_DEPTH_TEST);
    if(!cullFaceEnabled) glDisable(GL_CULL_FACE);

}

std::shared_ptr<Chunk> World::getChunk(glm::ivec3 chunkCoordinates) {
    int a = std::abs(chunkCoordinates.x % 16);
    int b = std::abs(chunkCoordinates.y % 16);
    int c = std::abs(chunkCoordinates.z % 16);
    std::vector<std::shared_ptr<Chunk>> &list = this->chunks[a][b][c];
    for(std::shared_ptr<Chunk> chunk : list) {
        if(chunk == nullptr) continue;
        if(chunk->getChunkCoordinates().x == chunkCoordinates.x && chunk->getChunkCoordinates().y == chunkCoordinates.y && chunk->getChunkCoordinates().z == chunkCoordinates.z) {
            return chunk;
        }
    }
    return nullptr;
}

std::shared_ptr<Chunk> World::generateChunk(glm::ivec3 chunkCoordinates) {
    LOG_DF("Generation Chunk at %d %d %d", chunkCoordinates.x, chunkCoordinates.y, chunkCoordinates.z);
    std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(chunkCoordinates);
    int a = std::abs(chunkCoordinates.x % 16);
    int b = std::abs(chunkCoordinates.y % 16);
    int c = std::abs(chunkCoordinates.z % 16);
    std::vector<std::shared_ptr<Chunk>> &list = this->chunks[a][b][c];
    list.push_back(chunk);
    this->generator->generate(chunk);
    return chunk;
}

void World::chunkWorkerLoad(World *world) {

    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if(world->currentCamera == nullptr) {
            continue;
        }

        glm::ivec3 cameraChunkCoordinates = (world->currentCamera->position / glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z));

        loadChunk(world, cameraChunkCoordinates);

        for(int v = 0; v < 1; v ++) {
            int y = v / 2 * (v % 2 == 0 ? 1 : -1);
            for(int r = 0; r < world->renderDistance; r ++) {
                for(int x = -r; x <= r; x ++) {
                    glm::ivec3 chunkCoords = {cameraChunkCoordinates.x + x, cameraChunkCoordinates.y + y, cameraChunkCoordinates.z + r};
                    loadChunk(world, chunkCoords);
                }
                for(int z = r - 1; z >= -r; z --) {
                    glm::ivec3 chunkCoords = {cameraChunkCoordinates.x + r, cameraChunkCoordinates.y + y, cameraChunkCoordinates.z + z};
                    loadChunk(world, chunkCoords);
                }
                for(int x = r - 1; x >= -r; x --) {
                    glm::ivec3 chunkCoords = {cameraChunkCoordinates.x + x, cameraChunkCoordinates.y + y, cameraChunkCoordinates.z - r};
                    loadChunk(world, chunkCoords);
                }
                for(int z = -r; z <= r-1; z ++) {
                    glm::ivec3 chunkCoords = {cameraChunkCoordinates.x - r, cameraChunkCoordinates.y + y, cameraChunkCoordinates.z + z};
                    loadChunk(world, chunkCoords);
                }
            }
        }
    }
}

void World::loadChunk(World* world, glm::ivec3 chunkCoords) {
    std::shared_ptr<Chunk> chunk = world->getChunk(chunkCoords);

    if(world->currentCamera == nullptr) return;
    if(chunk == nullptr) {
        World::queueChunkGeneration(world, chunkCoords);
        return;
    }
    if(!chunk->isInFrustum(world->currentCamera->frustum)) {
        return;
    }

    world->loadedChunksLock.lock();
    if(std::find(world->loadedChunks.begin(), world->loadedChunks.end(), chunk) != world->loadedChunks.end()) {
        world->loadedChunksLock.unlock();
        return;
    }
    world->loadedChunks.push_back(chunk);
    world->loadedChunksLock.unlock();
}

void World::chunkWorkerGenerate(World* world) {
    while(true) {
        if(world->queueChunkGenerate.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }
        glm::ivec3 chunkCoordinates = world->queueChunkGenerate.front();
        world->queueChunkGenerate.erase(world->queueChunkGenerate.begin());

        glm::vec3 chunkMinP = glm::vec3(chunkCoordinates.x * CHUNK_SIZE_X, chunkCoordinates.y * CHUNK_SIZE_Y, chunkCoordinates.z * CHUNK_SIZE_Z);
        glm::vec3 chunkMaxP = chunkMinP + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
        if(!world->currentCamera->frustum.IsBoxVisible(chunkMinP, chunkMaxP)) {
            continue;
        }

        std::shared_ptr<Chunk> chunk = world->generateChunk(chunkCoordinates);

        world->loadedChunksLock.lock();
        world->loadedChunks.push_back(chunk);
        world->loadedChunksLock.unlock();
    }
}

void World::queueChunkGeneration(World* world, glm::ivec3 chunkCoordinates) {
    if(std::find(world->queueChunkGenerate.begin(), world->queueChunkGenerate.end(), chunkCoordinates) != world->queueChunkGenerate.end()) {
        return;
    }

    world->queueChunkGenerate.push_back(chunkCoordinates);

    std::sort(world->queueChunkGenerate.begin(), world->queueChunkGenerate.end(), [world](glm::ivec3 a, glm::ivec3 b) {
        glm::vec3 aPos = glm::vec3(a.x * CHUNK_SIZE_X, a.y * CHUNK_SIZE_Y, a.z * CHUNK_SIZE_Z) + glm::vec3(CHUNK_SIZE_X / 2, CHUNK_SIZE_Y / 2, CHUNK_SIZE_Z / 2);
        glm::vec3 bPos = glm::vec3(b.x * CHUNK_SIZE_X, b.y * CHUNK_SIZE_Y, b.z * CHUNK_SIZE_Z) + glm::vec3(CHUNK_SIZE_X / 2, CHUNK_SIZE_Y / 2, CHUNK_SIZE_Z / 2);
        float aDist = glm::distance(world->currentCamera->position, aPos);
        float bDist = glm::distance(world->currentCamera->position, bPos);
        return aDist < bDist;
    });

}