//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_CHUNK_HPP
#define PM_LIKE_CHUNK_HPP

#include <iostream>
#include <mutex>
#include <memory>
#include "glm/glm.hpp"
#include "render/camera/Camera.hpp"
#include "render/ShaderProgram.hpp"
#include "world/material/Material.hpp"
#include "glad/glad.h"
#include "render/Texture.hpp"

#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32
#define VERTEX_SIZE (5*sizeof(float) + sizeof(uint8_t))

namespace pmlike::world {

    const uint8_t BLOCK_X_POS = 0b00000001;
    const uint8_t BLOCK_X_NEG = 0b00000010;
    const uint8_t BLOCK_Y_POS = 0b00000100;
    const uint8_t BLOCK_Y_NEG = 0b00001000;
    const uint8_t BLOCK_Z_POS = 0b00010000;
    const uint8_t BLOCK_Z_NEG = 0b00100000;

    class Chunk {
        public:
            static pmlike::render::ShaderProgram *shaderProgram;
            static pmlike::render::Texture* textureAtlas;

            bool destroyed = false;

            Chunk(glm::ivec3 chunkCoordinates);

            ~Chunk();

            glm::ivec3 getChunkCoordinates();

            bool generated = false;
            bool generationQueued = false;

            void render(std::shared_ptr<render::Camera> &camera, double deltaTime);

            void updateMesh(bool updateNeighbours);
            void unloadMesh();

            world::material::block::BlockMaterial blockAt(glm::ivec3 blockChunkCoords);
            void blockAt(glm::ivec3 blockChunkCoords, world::material::block::BlockMaterial block);

            bool isInFrustum(Frustum &frustum);

        private:
            static void initShaderProgram();

            bool dirty = true;

            glm::ivec3 coordinate;
            glm::vec3 minP, maxP;
            glm::mat4 transform;

            std::mutex glDataMutex;
            GLuint vao = 0, vbo = 0;
            GLsizeiptr numberVertices = 0;
            uint8_t* vertices = nullptr;

            world::material::block::BlockMaterial blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

            void copyToGPU();

            /**
             * Returns exposed sides
             * @param block
             * @return bitmask for sides
             */
            uint8_t getVisibleSides(glm::ivec3 blockChunkCoords);

    };

}

#endif //PM_LIKE_CHUNK_HPP
