#version 330 core

in vec2 texCoords;

uniform sampler2D textureAtlas;

out vec4 FragColor;

void main() {
    if(texCoords.x == -1 && texCoords.y == -1) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }
    vec4 textureColor = texture2D(textureAtlas, texCoords);
    FragColor = textureColor;
}
