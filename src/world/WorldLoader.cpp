//
// Created by finnw on 02.06.2023.
//

#include <thread>
#include <algorithm>
#include "World.hpp"
#include "glm/gtx/norm.hpp"
#include "util/Log.hpp"

using namespace pmlike::world;

void World::start() {
    if (this->generator == nullptr) {
        LOG_E("No generator set! Use World::setGenerator() to set a generator before starting the world.");
        throw std::runtime_error("No generator set");
    }

    this->running = true;
    this->generateQueue = std::vector<GeneratorQueue>(this->numberGeneratorThreads);
    this->generatorThreads = std::vector<std::thread>();

    for (int i = 0; i < this->numberGeneratorThreads; i++) {
        this->generateQueue.emplace_back();
        this->generatorThreads.emplace_back(World::chunkGeneratorWorker, i);
    }
    this->loaderThread = std::thread(World::chunkLoaderWorker);
}

void World::chunkGeneratorWorker(int workerId) {
    std::shared_ptr<World> world = World::getInstance();

    while (world->running && world->numberGeneratorThreads > workerId) {
        GeneratorQueue &queue = world->generateQueue[workerId];
        queue.mutex.lock();
        std::vector<std::shared_ptr<Chunk>> &chunks = queue.chunks;

        if (chunks.empty()) {
            chunks.shrink_to_fit();
            queue.mutex.unlock(); //Done with operations on queue
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Ticking every 50ms or 20 times per second
            continue;
        }
        glm::ivec3 cameraChunk = glm::ivec3(world->currentCamera->position.x / CHUNK_SIZE_X,
                                            world->currentCamera->position.y / CHUNK_SIZE_Y,
                                            world->currentCamera->position.z / CHUNK_SIZE_Z);

        std::sort(chunks.begin(), chunks.end(),
                  [cameraChunk](const std::shared_ptr<Chunk> &a, const std::shared_ptr<Chunk> &b) {
                      if (a == nullptr) return false;
                      if (b == nullptr) return true;
                      return glm::distance2(glm::vec3(a->getChunkCoordinates()), glm::vec3(cameraChunk)) <
                             glm::distance2(glm::vec3(b->getChunkCoordinates()), glm::vec3(cameraChunk));
                  });

        std::shared_ptr<Chunk> chunk = chunks.front();
        chunks.erase(chunks.begin());
        queue.mutex.unlock(); //Done with operations on queue

        if (chunk->generated) continue;
        if(!isChunkInFrustum(chunk->getChunkCoordinates())) {
            chunk->generationQueued = false;
            continue;
        }
        world->generator->generate(chunk);

        world->loadedChunksLock.lock();
        world->loadedChunks.push_back(chunk);
        world->loadedChunksLock.unlock();
    }
}

void World::chunkLoaderWorker() {
    std::shared_ptr<World> world = World::getInstance();

    static int nextWorker = 0;
    while (world->running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Ticking every 50ms or 20 times per second
        if (world->currentCamera == nullptr) continue;

        glm::ivec3 cameraChunk = glm::ivec3(world->currentCamera->position.x / CHUNK_SIZE_X,
                                            world->currentCamera->position.y / CHUNK_SIZE_Y,
                                            world->currentCamera->position.z / CHUNK_SIZE_Z);

        for (int x = cameraChunk.x - world->loadDistance.x; x <= cameraChunk.x + world->loadDistance.x; x++) {
            for (int y = cameraChunk.y - world->loadDistance.y; y <= cameraChunk.y + world->loadDistance.y; y++) {
                for (int z = cameraChunk.z - world->loadDistance.z; z <= cameraChunk.z + world->loadDistance.z; z++) {
                    glm::ivec3 chunkCoordinates = glm::ivec3(x, y, z);
                    if (isChunkInFrustum(chunkCoordinates) || (std::abs(x) <= 1 && std::abs(y) <= 1 &&
                                                               std::abs(z) <=
                                                               1)) { //Chunk is in Frustum or is a neighbor of the camera
                        std::shared_ptr<Chunk> chunk = world->getChunk(chunkCoordinates);
                        if (chunk != nullptr && chunk->generated) {
                            continue;
                        } else if (chunk == nullptr) {
                            chunk = std::make_shared<Chunk>(chunkCoordinates);
                            world->putChunk(chunk);
                        }
                        if(chunk->generationQueued) continue;
                        chunk->generationQueued = true;

                        world->generateQueue[nextWorker].mutex.lock();
                        world->generateQueue[nextWorker].chunks.push_back(chunk);
                        world->generateQueue[nextWorker].mutex.unlock();
                        nextWorker = (nextWorker + 1) % world->numberGeneratorThreads;
                    }
                }
            }
        }

        //Check for chunks to unload
        world->loadedChunksLock.lock();
        for (auto it = world->loadedChunks.begin(); it != world->loadedChunks.end();) {
            std::shared_ptr<Chunk> chunk = *it;
            if (chunk == nullptr) {
                it = world->loadedChunks.erase(it);
                continue;
            }
            if (!isChunkInFrustum(chunk->getChunkCoordinates()) && !chunk->generationQueued) {
                it = world->loadedChunks.erase(it);
                world->removeChunk(chunk);
                LOG_DF("Unloading Chunk (%d, %d, %d)", chunk->getChunkCoordinates().x, chunk->getChunkCoordinates().y, chunk->getChunkCoordinates().z);
            } else {
                it++;
            }
        }
        world->loadedChunksLock.unlock();

    }
}

bool World::isChunkInFrustum(glm::ivec3 chunkCoordinates) {
    std::shared_ptr<World> world = World::getInstance();

    glm::vec3 minP = glm::vec3(chunkCoordinates.x * CHUNK_SIZE_X, chunkCoordinates.y * CHUNK_SIZE_Y,
                               chunkCoordinates.z * CHUNK_SIZE_Z);
    glm::vec3 maxP = glm::vec3((chunkCoordinates.x + 1) * CHUNK_SIZE_X, (chunkCoordinates.y + 1) * CHUNK_SIZE_Y,
                               (chunkCoordinates.z + 1) * CHUNK_SIZE_Z);
    return world->currentCamera->frustum.IsBoxVisible(minP, maxP);
}

void World::setNumberGeneratorThreads(int nrGenThreads) {
    if (numberGeneratorThreads < 1) {
        throw std::invalid_argument("numberGeneratorThreads must be greater than 0");
    }
    if (nrGenThreads > this->numberGeneratorThreads) {
        for (int i = 0; i < nrGenThreads - this->numberGeneratorThreads; i++) {
            this->generateQueue.emplace_back();
            this->generatorThreads.emplace_back(World::chunkGeneratorWorker, this->numberGeneratorThreads + i);
        }
        this->numberGeneratorThreads = nrGenThreads;
    } else {
        int rm = this->numberGeneratorThreads - nrGenThreads;
        for (int i = 0; i < rm; i++) {
            std::thread &thread = this->generatorThreads.back();
            thread.join();
            this->generatorThreads.pop_back();
        }
        this->numberGeneratorThreads = nrGenThreads;
        this->generatorThreads.shrink_to_fit();
    }
}

int World::getNumberGeneratorThreads() {
    return this->numberGeneratorThreads;
}

size_t World::getNumberOfQueuedChunks() {
    std::shared_ptr<World> world = World::getInstance();
    int count = 0;
    for (int i = 0; i < world->numberGeneratorThreads; i++) {
        count += world->generateQueue[i].chunks.size();
    }
    return count;
}

size_t World::getNumberOfLoadedChunks() {
    return World::getInstance()->loadedChunks.size();
}
