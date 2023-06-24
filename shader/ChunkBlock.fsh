#version 330 core

in vec2 texCoords;

uniform sampler2D textureAtlas;

out vec4 FragColor;

void main() {
    vec4 textureColor = texture2D(textureAtlas, texCoords);
    FragColor = textureColor;
}
