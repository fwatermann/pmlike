#version 330 core

#define BLOCK_X_POS 1
#define BLOCK_X_NEG 2
#define BLOCK_Y_POS 4
#define BLOCK_Y_NEG 8
#define BLOCK_Z_POS 16
#define BLOCK_Z_NEG 32

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec2 textureAtlasSize;

in vec2 textureIndex[];
in int face[];

out vec2 texCoords;

void vertex(vec4 pos, vec4 offset, vec2 textureCoords);

void main() {

    vec4 pos = gl_in[0].gl_Position;
    vec2 textureStart = vec2(textureIndex[0].x * (1.0 / textureAtlasSize.x), textureIndex[0].y * (1.0 / textureAtlasSize.y));
    vec2 textureUnit = vec2((1.0f / 4.0f) / textureAtlasSize.x, (1.0f / 3.0f) / textureAtlasSize.y);

    //projection * view * model * vec4(aPosition, 1.0);

    if ((face[0] & BLOCK_X_POS) > 0) { // RIGHT
        vertex(pos, vec4(1, 1, 0, 0), textureStart + vec2(2 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(1, 1, 1, 0), textureStart + vec2(3 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(1, 0, 0, 0), textureStart + vec2(2 * textureUnit.x, 2 * textureUnit.y));
        vertex(pos, vec4(1, 0, 1, 0), textureStart + vec2(3 * textureUnit.x, 2 * textureUnit.y));
        EndPrimitive();
    }

    if ((face[0] & BLOCK_X_NEG) > 0) { // LEFT
        vertex(pos, vec4(0, 1, 1, 0), textureStart + vec2(0 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(0, 1, 0, 0), textureStart + vec2(1 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(0, 0, 1, 0), textureStart + vec2(0 * textureUnit.x, 2 * textureUnit.y));
        vertex(pos, vec4(0, 0, 0, 0), textureStart + vec2(1 * textureUnit.x, 2 * textureUnit.y));
        EndPrimitive();
    }

    if ((face[0] & BLOCK_Y_POS) > 0) { // TOP
        vertex(pos, vec4(0, 1, 0, 0), textureStart + vec2(1 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(0, 1, 1, 0), textureStart + vec2(1 * textureUnit.x, 0 * textureUnit.y));
        vertex(pos, vec4(1, 1, 0, 0), textureStart + vec2(2 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(1, 1, 1, 0), textureStart + vec2(2 * textureUnit.x, 0 * textureUnit.y));
        EndPrimitive();
    }

    if ((face[0] & BLOCK_Y_NEG) > 0) { // BOTTOM
        vertex(pos, vec4(0, 0, 1, 0), textureStart + vec2(1 * textureUnit.x, 3 * textureUnit.y));
        vertex(pos, vec4(0, 0, 0, 0), textureStart + vec2(1 * textureUnit.x, 2 * textureUnit.y));
        vertex(pos, vec4(1, 0, 1, 0), textureStart + vec2(2 * textureUnit.x, 3 * textureUnit.y));
        vertex(pos, vec4(1, 0, 0, 0), textureStart + vec2(2 * textureUnit.x, 2 * textureUnit.y));
        EndPrimitive();
    }

    if ((face[0] & BLOCK_Z_POS) > 0) { // BACK
        vertex(pos, vec4(1, 1, 1, 0), textureStart + vec2(3 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(0, 1, 1, 0), textureStart + vec2(4 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(1, 0, 1, 0), textureStart + vec2(3 * textureUnit.x, 2 * textureUnit.y));
        vertex(pos, vec4(0, 0, 1, 0), textureStart + vec2(4 * textureUnit.x, 2 * textureUnit.y));
        EndPrimitive();
    }

    if ((face[0] & BLOCK_Z_NEG) > 0) { // FRONT
        vertex(pos, vec4(0, 1, 0, 0), textureStart + vec2(1 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(1, 1, 0, 0), textureStart + vec2(2 * textureUnit.x, 1 * textureUnit.y));
        vertex(pos, vec4(0, 0, 0, 0), textureStart + vec2(1 * textureUnit.x, 2 * textureUnit.y));
        vertex(pos, vec4(1, 0, 0, 0), textureStart + vec2(2 * textureUnit.x, 2 * textureUnit.y));
        EndPrimitive();
    }

}

void vertex(vec4 pos, vec4 offset, vec2 textureCoords) {
    gl_Position = projection * view * model * (pos + offset);
    if(textureIndex[0].x < 0 || textureIndex[0].y < 0) {
        texCoords = vec2(-1, -1);
    } else {
        texCoords = textureCoords;
    }
    EmitVertex();
}
