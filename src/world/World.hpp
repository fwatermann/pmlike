//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_WORLD_HPP
#define PM_LIKE_WORLD_HPP

#include <map>
#include <memory>
#include <thread>
#include "Chunk.hpp"
#include "./generator/ChunkGenerator.hpp"
#include "./generator/TestGenerator.hpp"
#include "../render/camera/Camera.hpp"

#define CHUNK_TREE_BUCKET_SIZE 16

namespace pmlike::world {

    struct GeneratorQueue {

        GeneratorQueue() = default;
        GeneratorQueue(const GeneratorQueue& other) {
            this->chunks = other.chunks;
        }

        std::mutex mutex;
        std::vector<std::shared_ptr<Chunk>> chunks;
    };

    class World {

        public:

            static std::shared_ptr<World> getInstance();
            static void deleteInstance();

            int renderedChunks = 0;

            ~World();

            void start();
            void render(std::shared_ptr<render::Camera> camera, double deltaTime);
            void putChunk(const std::shared_ptr<Chunk>& chunk);
            std::shared_ptr<Chunk> getChunk(glm::ivec3 chunkCoordinates);

            void setLoadDistance(glm::ivec3 d);
            glm::ivec3 getLoadDistance();
            void setNumberGeneratorThreads(int numberGeneratorThreads);
            int getNumberGeneratorThreads();
            void setGenerator(std::shared_ptr<ChunkGenerator> generator);
            size_t getNumberOfQueuedChunks();
            size_t getNumberOfLoadedChunks();

        private:

            static std::shared_ptr<World> instance;
            World();

            std::vector<std::shared_ptr<Chunk>> chunks[CHUNK_TREE_BUCKET_SIZE][CHUNK_TREE_BUCKET_SIZE][CHUNK_TREE_BUCKET_SIZE];
            std::mutex chunksLock[CHUNK_TREE_BUCKET_SIZE][CHUNK_TREE_BUCKET_SIZE][CHUNK_TREE_BUCKET_SIZE];
            std::shared_ptr<render::Camera> currentCamera = nullptr;
            std::shared_ptr<ChunkGenerator> generator;

            /* CHUNK LOADING */

            bool running = false;
            int numberGeneratorThreads = 4;
            glm::ivec3 loadDistance = {1, 4, 1};

            std::mutex loadedChunksLock;
            std::vector<std::shared_ptr<Chunk>> loadedChunks;

            std::vector<GeneratorQueue> generateQueue;
            std::vector<std::thread> generatorThreads;
            std::thread loaderThread;


            static void chunkGeneratorWorker(int workerId);
            static void chunkLoaderWorker();
            static bool isChunkInFrustum(glm::ivec3 chunkCoordinates);

    };

}

#endif //PM_LIKE_WORLD_HPP
