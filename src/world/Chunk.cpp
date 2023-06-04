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
    this->transform = glm::translate(glm::mat4(1.0f),
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
    glDeleteBuffers(1, &this->ebo);
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

    shaderProgram->use();
    shaderProgram->setUniformMat4("projection", camera->projectionMatrix);
    shaderProgram->setUniformMat4("view", camera->viewMatrix);
    shaderProgram->setUniformMat4("model", this->transform);

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    shaderProgram->unuse();
}

void Chunk::updateMesh() {
    this->glDataMutex.lock();
    this->vertices.clear();
    this->indices.clear();

    GLuint currentIndex = 0;

    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                world::block::Block *block = this->blocks[x][y][z];
                if (block == nullptr || block->material == block::AIR) {
                    continue;
                }

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z); //- - - 0
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z + 1); //- - + 1
                vertices.push_back(x + 1);
                vertices.push_back(y);
                vertices.push_back(z + 1); //+ - + 2
                vertices.push_back(x + 1);
                vertices.push_back(y);
                vertices.push_back(z); //+ - - 3
                vertices.push_back(x);
                vertices.push_back(y + 1);
                vertices.push_back(z); //- + - 4
                vertices.push_back(x);
                vertices.push_back(y + 1);
                vertices.push_back(z + 1); //- + + 5
                vertices.push_back(x + 1);
                vertices.push_back(y + 1);
                vertices.push_back(z + 1); //+ + + 6
                vertices.push_back(x + 1);
                vertices.push_back(y + 1);
                vertices.push_back(z); //+ + - 7

                uint8_t visibleFaces = getVisibleSides(block);

                //x+
                if ((visibleFaces & BLOCK_X_POS) == BLOCK_X_POS) {
                    indices.push_back(currentIndex + 6);
                    indices.push_back(currentIndex + 2);
                    indices.push_back(currentIndex + 3);
                    indices.push_back(currentIndex + 3);
                    indices.push_back(currentIndex + 7);
                    indices.push_back(currentIndex + 6);
                }
                //x-
                if ((visibleFaces & BLOCK_X_NEG) == BLOCK_X_NEG) {
                    indices.push_back(currentIndex + 5);
                    indices.push_back(currentIndex + 4);
                    indices.push_back(currentIndex + 0);
                    indices.push_back(currentIndex + 0);
                    indices.push_back(currentIndex + 1);
                    indices.push_back(currentIndex + 5);
                }
                //y+
                if ((visibleFaces & BLOCK_Y_POS) == BLOCK_Y_POS) {
                    indices.push_back(currentIndex + 6);
                    indices.push_back(currentIndex + 7);
                    indices.push_back(currentIndex + 4);
                    indices.push_back(currentIndex + 4);
                    indices.push_back(currentIndex + 5);
                    indices.push_back(currentIndex + 6);
                }
                //y-
                if ((visibleFaces & BLOCK_Y_NEG) == BLOCK_Y_NEG) {
                    indices.push_back(currentIndex + 2);
                    indices.push_back(currentIndex + 1);
                    indices.push_back(currentIndex + 0);
                    indices.push_back(currentIndex + 0);
                    indices.push_back(currentIndex + 3);
                    indices.push_back(currentIndex + 2);
                }
                //z+
                if ((visibleFaces & BLOCK_Z_POS) == BLOCK_Z_POS) {
                    indices.push_back(currentIndex + 6);
                    indices.push_back(currentIndex + 5);
                    indices.push_back(currentIndex + 1);
                    indices.push_back(currentIndex + 1);
                    indices.push_back(currentIndex + 2);
                    indices.push_back(currentIndex + 6);
                }
                //z-
                if ((visibleFaces & BLOCK_Z_NEG) == BLOCK_Z_NEG) {
                    indices.push_back(currentIndex + 7);
                    indices.push_back(currentIndex + 3);
                    indices.push_back(currentIndex + 0);
                    indices.push_back(currentIndex + 0);
                    indices.push_back(currentIndex + 4);
                    indices.push_back(currentIndex + 7);
                }

                currentIndex += 8;
            }
        }
    }
    this->dirty = true;
    this->glDataMutex.unlock();
}

void Chunk::copyToGPU() {
    this->glDataMutex.lock();
    this->indexCount = indices.size();

    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid *) 0);
    glEnableVertexAttribArray(shaderProgram->getAttribLocation("aPosition"));

    glBindVertexArray(0);

    this->vertices.clear();
    this->indices.clear();
    this->vertices.shrink_to_fit();
    this->indices.shrink_to_fit();
    this->dirty = false;
    this->glDataMutex.unlock();
    LOG_DF("Copied Chunk (%d %d %d) to GPU", this->coordinate.x, this->coordinate.y, this->coordinate.z);
}

void Chunk::initShaderProgram() {
    pmlike::render::Shader vertexShader = pmlike::render::Shader("shader/Simple.vert", GL_VERTEX_SHADER, true);
    pmlike::render::Shader fragmentShader = pmlike::render::Shader("shader/Simple.frag", GL_FRAGMENT_SHADER, true);
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
