//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_CHUNK_HPP
#define PM_LIKE_CHUNK_HPP

#include <iostream>
#include <mutex>
#include "glm/glm.hpp"
#include "./block/Block.hpp"
#include "render/camera/Camera.hpp"
#include "../render/ShaderProgram.hpp"
#include "glad/glad.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

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

            Chunk(glm::ivec3 chunkCoordinates);

            ~Chunk();

            world::block::Block *blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

            glm::ivec3 getChunkCoordinates();

            bool generated = false;
            bool generationQueued = false;

            void render(std::shared_ptr<render::Camera> &camera, double deltaTime);

            void updateMesh();

            bool isInFrustum(Frustum &frustum);

        private:
            static void initShaderProgram();

            bool dirty = true;

            glm::ivec3 coordinate;
            glm::vec3 minP, maxP;

            GLuint vao = 0, vbo = 0, ebo = 0;
            GLsizei indexCount;

            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            std::mutex glDataMutex;

            glm::mat4 transform;

            void copyToGPU();

            /**
             * Returns exposed sides
             * @param block
             * @return bitmask for sides
             */
            uint8_t getVisibleSides(block::Block *block);

    };

}

#endif //PM_LIKE_CHUNK_HPP
