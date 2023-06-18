//
// Created by finnw on 31.05.2023.
//

#include <vector>
#include <cstring>
#include "Chunk.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "util/Log.hpp"
#include "../render/Texture.hpp"

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
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
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
    if (this->dirty) {
        this->copyToGPU();
    }
    if(this->numberVisibleBlocks <= 0) return;

    //TODO: Replace
    if(block::Block::texture == nullptr) {
        block::Block::texture = render::Texture::fromFile("assets/textures/grass_block.png");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderProgram->use();
    shaderProgram->setUniformMat4("projection", camera->projectionMatrix);
    shaderProgram->setUniformMat4("view", camera->viewMatrix);
    shaderProgram->setUniformMat4("model", this->chunkTransform);
    shaderProgram->setUniform2i("atlasSize", 1, 1);
    block::Block::texture->bindToProgram(GL_TEXTURE0, shaderProgram, "textureAtlas");

    glBindVertexArray(this->vao);
    glDrawArrays(GL_POINTS, 0, this->numberVisibleBlocks);
    glBindVertexArray(0);

    shaderProgram->unuse();
}

void Chunk::updateMesh() {
    //Size of Instance Data in Bytes = 21 bytes (3 floats for position, 2 floats for texture coordinates, 1 byte for visible faces mask)
    this->instanceDataMutex.lock();
    if(this->instanceData != nullptr) {
        std::free(this->instanceData);
    }
    this->instanceData = (uint8_t*) std::calloc(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, 5*sizeof(float) + sizeof(uint8_t));
    const GLuint stride = 5*sizeof(float) + 1;

    for(int x = 0; x < CHUNK_SIZE_X; x++) {
        for(int y = 0; y < CHUNK_SIZE_Y; y++) {
            for(int z = 0; z < CHUNK_SIZE_Z; z++) {
                if(this->blocks[x][y][z] != nullptr) {
                    block::Block* block = this->blocks[x][y][z];
                    if(block->material == block::BlockMaterial::AIR) continue;

                    uint8_t visibleFacesMask = getVisibleSides(block);
                    if(visibleFacesMask == 0) continue;

                    glm::ivec3 pos = glm::ivec3 {x, y, z};
                    glm::ivec2 textureAtlasCoords = glm::ivec2(0, 0);
                    std::memcpy(this->instanceData + this->numberVisibleBlocks * stride, &pos, 3 * sizeof(float));
                    std::memcpy(this->instanceData + this->numberVisibleBlocks * stride + 3*sizeof(float), &textureAtlasCoords, 2 * sizeof(float));
                    std::memcpy(this->instanceData + this->numberVisibleBlocks * stride + 5*sizeof(float), &visibleFacesMask, sizeof(uint8_t));

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

    if(this->vbo == 0) {
        glGenBuffers(1, &this->vbo);
    }

    const GLuint stride = 5*sizeof(float) + 1;

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    this->instanceDataMutex.lock();
    glBufferData(GL_ARRAY_BUFFER, this->numberVisibleBlocks * stride, this->instanceData, GL_STATIC_DRAW);
    this->instanceDataMutex.unlock();

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aPosition"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, stride, (void*) 0);

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aAtlasCoords"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aAtlasCoords"), 2, GL_FLOAT, GL_FALSE, stride, (void*) (3*sizeof(float)));

    glEnableVertexAttribArray(Chunk::shaderProgram->getAttribLocation("aFacesMask"));
    glVertexAttribPointer(Chunk::shaderProgram->getAttribLocation("aFacesMask"), 1, GL_UNSIGNED_BYTE, GL_FALSE, stride, (void*) (5*sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::free(this->instanceData);
    this->instanceData = nullptr;
    this->dirty = false;

    LOG_DF("Copied chunk (%d %d %d) to GPU (%d blocks)", this->coordinate.x, this->coordinate.y, this->coordinate.z, this->numberVisibleBlocks);

}

void Chunk::initShaderProgram() {
    render::Shader vertexShader = pmlike::render::Shader("shader/Chunk.vert", GL_VERTEX_SHADER, true);
    render::Shader fragmentShader = pmlike::render::Shader("shader/Chunk.frag", GL_FRAGMENT_SHADER, true);
    render::Shader geometryShader = render::Shader("shader/Chunk.geom", GL_GEOMETRY_SHADER, true);
    shaderProgram = new render::ShaderProgram(&vertexShader, &fragmentShader, &geometryShader);
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
