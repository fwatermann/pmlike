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
pmlike::render::Texture* Chunk::textureAtlas = nullptr;

Chunk::Chunk(glm::ivec3 chunkCoordinates) : coordinate(chunkCoordinates) {
    this->transform = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(chunkCoordinates.x * CHUNK_SIZE_X, chunkCoordinates.y * CHUNK_SIZE_Y,
                                               chunkCoordinates.z * CHUNK_SIZE_Z));
    this->minP = glm::vec3(chunkCoordinates.x * CHUNK_SIZE_X, chunkCoordinates.y * CHUNK_SIZE_Y,
                           chunkCoordinates.z * CHUNK_SIZE_Z);
    this->maxP = this->minP + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
    std::memset(this->blocks, 0, sizeof(this->blocks));
}

Chunk::~Chunk() {
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
    if (!this->generated || this->numberVertices == 0) {
        return;
    }
    if (this->dirty) {
        this->copyToGPU();
    }

    shaderProgram->use();
    shaderProgram->setUniformMat4("projection", camera->projectionMatrix);
    shaderProgram->setUniformMat4("view", camera->viewMatrix);
    shaderProgram->setUniformMat4("model", this->transform);
    shaderProgram->setUniform2f("textureAtlasSize", 2.0f, 1.0f);

    textureAtlas->bindToProgram(GL_TEXTURE0, shaderProgram, "textureAtlas");

    glBindVertexArray(this->vao);
    glDrawArrays(GL_POINTS, 0, this->numberVertices);
    glBindVertexArray(0);

    shaderProgram->unuse();
}

void Chunk::updateMesh() {
    this->glDataMutex.lock();

    std::free(this->vertices);
    this->vertices = (uint8_t*) std::calloc(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, VERTEX_SIZE);

    this->numberVertices = 0;
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                material::block::BlockMaterial block = this->blocks[x][y][z];
                uint8_t visibleFaces = getVisibleSides(glm::ivec3(x, y, z));
                if (block == material::block::BlockMaterial::AIR || visibleFaces == 0x00) {
                    continue;
                }
                glm::vec3 pos = glm::vec3(x, y, z);
                glm::vec2 textureIndex = glm::vec2(0, 0); //TODO: Get Texture Index based on Atlas
                std::memcpy(this->vertices + this->numberVertices * VERTEX_SIZE, &pos, 3 * sizeof(float));
                std::memcpy(this->vertices + this->numberVertices * VERTEX_SIZE + 3 * sizeof(float), &textureIndex, 2 * sizeof(float));
                std::memcpy(this->vertices + this->numberVertices * VERTEX_SIZE + 5 * sizeof(float), &visibleFaces, sizeof(uint8_t));

                this->numberVertices ++;
            }
        }
    }
    if(this->numberVertices == 0) {
        this->glDataMutex.unlock();
        return;
    }

    this->dirty = true;
    this->glDataMutex.unlock();
}

void Chunk::copyToGPU() {
    this->glDataMutex.lock();

    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->numberVertices * VERTEX_SIZE, vertices, GL_STATIC_DRAW);

    GLsizei stride = (3 + 2) * sizeof(float) + 1 * sizeof(uint8_t);

    glEnableVertexAttribArray(shaderProgram->getAttribLocation("aPosition"));
    glVertexAttribPointer(shaderProgram->getAttribLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *) 0);

    glEnableVertexAttribArray(shaderProgram->getAttribLocation("aTextureIndex"));
    glVertexAttribPointer(shaderProgram->getAttribLocation("aTextureIndex"), 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *) (3 * sizeof(float)));

    glEnableVertexAttribArray(shaderProgram->getAttribLocation("aVisibleFaces"));
    glVertexAttribIPointer(shaderProgram->getAttribLocation("aVisibleFaces"), 1, GL_UNSIGNED_BYTE, stride, (GLvoid *) (5 * sizeof(float)));

    glBindVertexArray(0);

    std::free(this->vertices);
    this->vertices = nullptr;
    this->dirty = false;
    this->glDataMutex.unlock();
    LOG_DF("Copied Chunk (%d %d %d) to GPU", this->coordinate.x, this->coordinate.y, this->coordinate.z);
}

void Chunk::unloadMesh() {
    this->glDataMutex.lock();
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    this->dirty = false;
    this->generated = false;
    this->numberVertices = 0;
    this->glDataMutex.unlock();
}

void Chunk::initShaderProgram() {
    pmlike::render::Shader vertexShader = pmlike::render::Shader("shader/ChunkBlock.vsh", GL_VERTEX_SHADER, true);
    pmlike::render::Shader fragmentShader = pmlike::render::Shader("shader/ChunkBlock.fsh", GL_FRAGMENT_SHADER, true);
    pmlike::render::Shader geometryShader = pmlike::render::Shader("shader/ChunkBlock.gsh", GL_GEOMETRY_SHADER, true);
    shaderProgram = new pmlike::render::ShaderProgram(&vertexShader, &fragmentShader, &geometryShader);

    textureAtlas = pmlike::render::Texture::fromFile("assets/textures/block/atlas.png");

}

uint8_t Chunk::getVisibleSides(glm::ivec3 blockChunkCoords) {
    const int x = blockChunkCoords.x;
    const int y = blockChunkCoords.y;
    const int z = blockChunkCoords.z;

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
    if (x > 0 && (material::block::isTransparent(blocks[x - 1][y][z]))) {
        ret |= BLOCK_X_NEG;
    }
    if (x < CHUNK_SIZE_X - 1 && (material::block::isTransparent(this->blocks[x + 1][y][z]))) {
        ret |= BLOCK_X_POS;
    }
    if (y > 0 && (material::block::isTransparent(this->blocks[x][y - 1][z]))) {
        ret |= BLOCK_Y_NEG;
    }
    if (y < CHUNK_SIZE_Y - 1 && (material::block::isTransparent(this->blocks[x][y + 1][z]))) {
        ret |= BLOCK_Y_POS;
    }
    if (z > 0 && (material::block::isTransparent(this->blocks[x][y][z - 1]))) {
        ret |= BLOCK_Z_NEG;
    }
    if (z < CHUNK_SIZE_Z - 1 && (material::block::isTransparent(this->blocks[x][y][z + 1]))) {
        ret |= BLOCK_Z_POS;
    }

    return ret;
}

bool Chunk::isInFrustum(Frustum &frustum) {
    return frustum.IsBoxVisible(this->minP, this->maxP);
}
