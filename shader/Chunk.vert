#version 330 core

layout (location = 0) in vec3 aPosition; //Vertex position rel to block pos
layout (location = 1) in vec2 aTexCoord; //Texture coordinates
layout (location = 2) in ivec3 aBlockPosition; //Block position in chunk
layout (location = 3) in ivec2 aTextureAtlasCoords; //Texture atlas coords

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform ivec2 atlasSize;

out float y;
out vec2 texCoord;
out ivec2 textureAtlasCoords;
flat out int faceMask;

void main() {
    y = aBlockPosition.y;
    gl_Position = projection * view * model * vec4(aPosition + aBlockPosition, 1.0);
    //TODO: LOAD TEXTURE FROM TEXTURE ATLAS
}
