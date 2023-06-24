#version 330 core

#define BLOCK_X_POS 1
#define BLOCK_X_NEG 2
#define BLOCK_Y_POS 4
#define BLOCK_Y_NEG 8
#define BLOCK_Z_POS 16
#define BLOCK_Z_NEG 32

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureIndex;
layout (location = 2) in int aVisibleFaces;

out vec2 textureIndex;
out int face;

void main() {
    textureIndex = aTextureIndex;
    face = aVisibleFaces;
    gl_Position = vec4(aPosition, 1.0f);
}
