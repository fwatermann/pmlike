//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_WORLD_HPP
#define PM_LIKE_WORLD_HPP

#include <map>
#include <memory>
#include <thread>
#include "Chunk.hpp"
#include "render/camera/Camera.hpp"
#include "world/generator/ChunkGenerator.hpp"
#include "./generator/TestGenerator.hpp"

namespace pmlike::world {


    class World {

        public:

            int renderDistance = 8;
            int renderedChunks = 0;

            explicit World(ChunkGenerator *generator);

            World();

            ~World();

            void render(render::Camera *camera, double deltaTime);

            std::shared_ptr<Chunk> getChunk(glm::ivec3 chunkCoordinates);

            std::shared_ptr<Chunk> generateChunk(glm::ivec3 chunkCoordinates);

        private:
            world::ChunkGenerator *generator = nullptr;
            render::Camera *currentCamera = nullptr;
            std::vector<std::shared_ptr<Chunk>> chunks[16][16][16];

            std::mutex loadedChunksLock;
            std::vector<std::shared_ptr<Chunk>> loadedChunks;
            std::vector<glm::ivec3> queueChunkGenerate;

            std::thread threadChunkLoad;
            std::thread threadChunkGenerate;

            static void loadChunk(World *world, glm::ivec3 chunkCoordinates);

            static void chunkWorkerLoad(World *world);

            static void chunkWorkerGenerate(World *world);

            static void queueChunkGeneration(World *world, glm::ivec3 chunkCoordinates);
    };

}

#endif //PM_LIKE_WORLD_HPP
