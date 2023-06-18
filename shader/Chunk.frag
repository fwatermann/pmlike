#version 330 core

in vec2 vTextCoords;
in vec3 vNormal;
in vec2 vAtlasCoords;

uniform ivec2 atlasSize;
uniform sampler2D textureAtlas;

out vec4 fragColor;

void main() {
    vec2 texCoords = vec2(vAtlasCoords.x, vAtlasCoords.y) / vec2(atlasSize.x, atlasSize.y);
    fragColor = texture(textureAtlas, texCoords);
}
