#version 330 core

layout (location = 0) in vec4 aVertex;
out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 offset;

void main() {
    gl_Position = projection * vec4(aVertex.xy + offset, 0.0, 1.0);
    TexCoords = aVertex.zw;
}