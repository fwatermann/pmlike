#version 330 core

layout (location = 0) in vec3 aPosition; //Vertex position rel to block pos


out float y;
out vec2 texCoord;

void main() {
    aPosition.y;
    gl_Position = vec4(aPosition, 1.0f);
    //TODO: LOAD TEXTURE FROM TEXTURE ATLAS
}
