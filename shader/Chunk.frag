#version 330 core

in float y;
in vec2 texCoord;
in ivec2 textureAtlasCoords;

out vec4 fragColor;

void main() {
    fragColor = vec4(y / 16.0f, 0.0f, 0.5f, 1.0f);
}
