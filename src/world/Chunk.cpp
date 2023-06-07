//
// Created by finnw on 31.05.2023.
//

#include <vector>
#include <cstring>
#include "Chunk.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "util/Log.hpp"

using namespace pmlike::world;

pmlike::render::ShaderProgram *Chunk::shaderProgram = nullptr;

Chunk::Chunk(glm::ivec3 chunkCoordinates) : coordinate(chunkCoordinates) {
    this->chunkTransform = glm::translate(glm::mat4(1.0f),
                                          glm::vec3(chunkCoordinates.x * CHUNK_SIZE_X, chunkCoordinates.y * CHUNK_SIZE_Y,
                                               chunkCoordinates.z * CHUNK_SIZE_Z));
    this->minP = glm::vec3(chunkCoordinates.x * CHUNK_SIZE_X, chunkCoordinates.y * CHUNK_SIZE_Y,
                           chunkCoordinates.z * CHUNK_SIZE_Z);
    this->maxP = this->minP + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
    std::memset(this->blocks, 0, sizeof(this->blocks));
}

Chunk::~Chunk() {
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                delete this->blocks[x][y][z];
            }
        }
    }
    glDeleteVertexArrays(1, &this->blockMeshData.vao);
    glDeleteBuffers(1, &this->blockMeshData.vbo);
};

glm::ivec3 Chunk::getChunkCoordinates() {
    return {(this->coordinate)};
}

void Chunk::render(std::shared_ptr<render::Camera> &camera, double deltaTime) {
    if (shaderProgram == nullptr) {
        Chunk::initShaderProgram();
    }
    if (!this->generated) {
        return;
    }
    if(this->blockMeshData.vao == 0) {
        this->initBlockMesh();
    }
    if (this->dirty) {
        this->copyToGPU();
    }
    if(this->numberVisibleBlocks <= 0) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderProgram->use();
    shaderProgram->setUniformMat4("projection", camera->projectionMatrix);
    shaderProgram->setUniformMat4("view", camera->viewMatrix);
    shaderProgram->setUniformMat4("model", this->chunkTransform);

    glBindVertexArray(this->blockMeshData.vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
    glBindVertexArray(0);

    shaderProgram->unuse();
}

void Chunk::updateMesh() {
    //Size of Instance Data in Bytes = 21 bytes (3 floats for position, 2 floats for texture coordinates, 1 byte for visible faces mask)
    this->instanceDataMutex.lock();
    if(this->instanceData != nullptr) {
        std::free(this->instanceData);
    }
    this->instanceData = (uint8_t*) std::calloc(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, 21);

    for(int x = 0; x < CHUNK_SIZE_X; x++) {
        for(int y = 0; y < CHUNK_SIZE_Y; y++) {
            for(int z = 0; z < CHUNK_SIZE_Z; z++) {
                if(this->blocks[x][y][z] != nullptr) {
                    block::Block* block = this->blocks[x][y][z];
                    if(block->material == block::BlockMaterial::AIR) continue;

                    uint8_t visibleFacesMask = getVisibleSides(block);
                    if(visibleFacesMask == 0) continue;

                    //Position
                    glm::ivec3 pos = glm::ivec3 {x, y, z};
                    std::memcpy(this->instanceData + (this->numberVisibleBlocks * 21), &pos, 12);

                    //Texture Coordinates
                    glm::vec2 texCoords = glm::ivec2(0, 0); //TODO: Replace with atlas texture coordinates
                    std::memcpy(this->instanceData + (this->numberVisibleBlocks * 21) + 12, &texCoords, 8);

                    //Visible faces mask
                    std::memcpy(this->instanceData + (this->numberVisibleBlocks * 21) + 20, &visibleFacesMask, 1);

                    this->numberVisibleBlocks ++;
                }
            }
        }
    }

    if(this->numberVisibleBlocks != 0) {
        this->dirty = true;
    } else {
        std::free(this->instanceData);
        this->instanceData = nullptr;
    }
    this->instanceDataMutex.unlock();
}

void Chunk::copyToGPU() {

    if(this->ibo != 0) {
        glDeleteBuffers(1, &this->ibo);
    }

    glGenBuffers(1, &this->ibo);
    glBindBuffer(GL_ARRAY_BUFFER, this->ibo);
    this->instanceDataMutex.lock();
    glBufferData(GL_ARRAY_BUFFER, this->numberVisibleBlocks * 21, this->instanceData, GL_STATIC_DRAW);
    this->instanceDataMutex.unlock();

    glBindVertexArray(this->blockMeshData.vao);

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aBlockPosition"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aBlockPosition"), 3, GL_FLOAT, GL_FALSE, 21, (void*) 0);
    glVertexAttribDivisor(Chunk::shaderProgram->getAttribLocation("aBlockPosition"), 1);

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aTextureAtlasCoords"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aTextureAtlasCoords"), 2, GL_FLOAT, GL_FALSE, 21, (void*) 12);
    glVertexAttribDivisor(Chunk::shaderProgram->getAttribLocation("aTextureAtlasCoords"), 1);

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aFacesMask"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aFacesMask"), 1, GL_UNSIGNED_BYTE, GL_FALSE, 21, (void*) 20);
    glVertexAttribDivisor(Chunk::shaderProgram->getAttribLocation("aFacesMask"), 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::free(this->instanceData);
    this->instanceData = nullptr;
    this->dirty = false;
}

void Chunk::initBlockMesh() {
    blockMeshData = BlockMeshData();

    float vertices[] = { //TODO: Texture Coordinates could be moved to shader due to being the same for every block.
            //y-
            0.0f, 0.0f, 0.0f,    0.25f, (2.0f/3.0f),  //0
            0.0f, 0.0f, 1.0f,    0.25f, 1.0f,  //3
            1.0f, 0.0f, 1.0f, 0.50f, 1.0f, //2
            1.0f, 0.0f, 1.0f, 0.50f, 1.0f, //2
            1.0f, 0.0f, 0.0f, 0.50f, (2.0f/3.0f), //1
            0.0f, 0.0f, 0.0f, 0.25f, (2.0f/3.0f), //0
            //y+
            0.0f, 1.0f, 0.0f, 0.25f, (1.0f/3.0f), //4
            1.0f, 1.0f, 0.0f, 0.5f, (1.0f/3.0f), //5
            1.0f, 1.0f, 1.0f, 0.5f, 0.0f, //6
            1.0f, 1.0f, 1.0f, 0.5f, 0.0f, //6
            0.0f, 1.0f, 1.0f, 0.25f, 0.0f, //7
            0.0f, 1.0f, 0.0f, 0.25f, (1.0f/3.0f), //4
            //x-
            0.0f, 0.0f, 0.0f, 0.25f, (2.0f/3.0f), //0
            0.0f, 1.0f, 0.0f, 0.25f, (1.0f/3.0f), //4
            0.0f, 1.0f, 1.0f, 0.0f, (1.0f/3.0f), //7
            0.0f, 1.0f, 1.0f, 0.0f, (1.0f/3.0f), //7
            0.0f, 0.0f, 1.0f, 0.0f, (2.0f/3.0f), //3
            0.0f, 0.0f, 0.0f, 0.25f, (2.0f/3.0f), //0
            //x+
            1.0f, 0.0f, 0.0f, 0.5f, (2.0f/3.0f), //1
            1.0f, 0.0f, 1.0f, 0.75f, (2.0f/3.0f), //2
            1.0f, 1.0f, 1.0f, 0.75f, (1.0f/3.0f), //6
            1.0f, 1.0f, 1.0f, 0.75f, (1.0f/3.0f), //6
            1.0f, 1.0f, 0.0f, 0.5f, (1.0f/3.0f), //5
            1.0f, 0.0f, 0.0f, 0.5f, (2.0f/3.0f), //1
            //z-
            0.0f, 0.0f, 0.0f, 0.25f, (2.0f/3.0f), //0
            1.0f, 0.0f, 0.0f, 0.5f, (2.0f/3.0f), //1
            1.0f, 1.0f, 0.0f, 0.5f, (1.0f/3.0f), //5
            1.0f, 1.0f, 0.0f, 0.5f, (1.0f/3.0f), //5
            0.0f, 1.0f, 0.0f, 0.25f, (1.0f/3.0f), //4
            0.0f, 0.0f, 0.0f, 0.25f, (2.0f/3.0f), //0
            //z+
            0.0f, 0.0f, 1.0f, 1.0f, (2.0f/3.0f), //3
            0.0f, 1.0f, 1.0f, 1.0f, (1.0f/3.0f), //7
            1.0f, 1.0f, 1.0f, 0.75f, (1.0f/3.0f), //6
            1.0f, 1.0f, 1.0f, 0.75f, (1.0f/3.0f), //6
            1.0f, 0.0f, 1.0f, 0.75f, (2.0f/3.0f), //2
            0.0f, 0.0f, 1.0f, 1.0f, (2.0f/3.0f), //3
    };

    glGenVertexArrays(1, &blockMeshData.vao);
    glBindVertexArray(blockMeshData.vao);

    glGenBuffers(1, &blockMeshData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, blockMeshData.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aPosition"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aTexCoord"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aTexCoord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Chunk::initShaderProgram() {
    pmlike::render::Shader vertexShader = pmlike::render::Shader("shader/Chunk.vert", GL_VERTEX_SHADER, true);
    pmlike::render::Shader fragmentShader = pmlike::render::Shader("shader/Chunk.frag", GL_FRAGMENT_SHADER, true);
    shaderProgram = new pmlike::render::ShaderProgram(&vertexShader, &fragmentShader);
}

uint8_t Chunk::getVisibleSides(block::Block *block) {
    const int x = block->getChunkCoordinate().x;
    const int y = block->getChunkCoordinate().y;
    const int z = block->getChunkCoordinate().z;

    uint8_t ret = 0b00000000;

    //Chunk borders
    if (x == 0) {
        ret |= BLOCK_X_NEG;
    }
    if (x == CHUNK_SIZE_X - 1) {
        ret |= BLOCK_X_POS;
    }
    if (y == 0) {
        ret |= BLOCK_Y_NEG;
    }
    if (y == CHUNK_SIZE_Y - 1) {
        ret |= BLOCK_Y_POS;
    }
    if (z == 0) {
        ret |= BLOCK_Z_NEG;
    }
    if (z == CHUNK_SIZE_Z - 1) {
        ret |= BLOCK_Z_POS;
    }

    //Block borders
    if (x > 0 && (this->blocks[x - 1][y][z] == nullptr || this->blocks[x - 1][y][z]->isTransparent())) {
        ret |= BLOCK_X_NEG;
    }
    if (x < CHUNK_SIZE_X - 1 && (this->blocks[x + 1][y][z] == nullptr || this->blocks[x + 1][y][z]->isTransparent())) {
        ret |= BLOCK_X_POS;
    }
    if (y > 0 && (this->blocks[x][y - 1][z] == nullptr || this->blocks[x][y - 1][z]->isTransparent())) {
        ret |= BLOCK_Y_NEG;
    }
    if (y < CHUNK_SIZE_Y - 1 && (this->blocks[x][y + 1][z] == nullptr || this->blocks[x][y + 1][z]->isTransparent())) {
        ret |= BLOCK_Y_POS;
    }
    if (z > 0 && (this->blocks[x][y][z - 1] == nullptr || this->blocks[x][y][z - 1]->isTransparent())) {
        ret |= BLOCK_Z_NEG;
    }
    if (z < CHUNK_SIZE_Z - 1 && (this->blocks[x][y][z + 1] == nullptr || this->blocks[x][y][z + 1]->isTransparent())) {
        ret |= BLOCK_Z_POS;
    }

    return ret;
}

bool Chunk::isInFrustum(Frustum &frustum) {
    return frustum.IsBoxVisible(this->minP, this->maxP);
}
