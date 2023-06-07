#version 330 core

#define BLOCK_X_POS 1
#define BLOCK_X_NEG 2
#define BLOCK_Y_POS 4
#define BLOCK_Y_NEG 8
#define BLOCK_Z_POS 16
#define BLOCK_Z_NEG 32

layout (point) in;
layout (triangle_strip, max_vertices = 36) out;

layout (location = 4) int aFacesMask;

out vec2 vTextCoords;
out vec3 vNormal;

void makeVertex(vec3 pos, vec2 texCoord);

void main() {

    if((aFacesMask & BLOCK_X_POS) == BLOCK_X_POS) {
        makeVertex(vec3(1.0f, 0.0f, 1.0f), vec2(0.75f, 2.0f/3.0f)); //2
        makeVertex(vec3(1.0f, 1.0f, 1.0f), vec2(0.75f, 1.0f/3.0f)); //6
        makeVertex(vec3(1.0f, 0.0f, 0.0f), vec2(0.5f, 2.0f/3.0f)); //1
        makeVertex(vec3(1.0f, 1.0f, 0.0f), vec2(0.5f, 1.0f/3.0f)); //5
        EmitPrimitive();
    }
    if((aFaceMask & BLOCK_X_NEG) == BLOCK_X_NEG) {
        makeVertex(vec3(0.0f, 0.0f, 0.0f), vec2(0.25f, 2.0f/3.0f)); //0
        makeVertex(vec3(0.0f, 1.0f, 0.0f), vec2(0.25f, 1.0f/3.0f)); //4
        makeVertex(vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 2.0f/3.0f)); //3
        makeVertex(vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f/3.0f)); //7
        EmitPrimitive();
    }
    if((aFacesMask & BLOCK_Y_POS) == BLOCK_Y_POS) {
        makeVertex(vec3(0.0f, 1.0f, 1.0f), vec2(0.25f, 0.0f)); //7
        makeVertex(vec3(0.0f, 1.0f, 0.0f), vec2(0.25f, 1.0f/3.0f)); //4
        makeVertex(vec3(1.0f, 1.0f, 1.0f), vec2(0.5f, 0.0f)); //6
        makeVertex(vec3(1.0f, 1.0f, 0.0f), vec2(0.5f, 1.0f/3.0f)); //5
        EmitPrimitive();
    }
    if((aFacesMask & BLOCK_Y_NEG) == BLOCK_Y_NEG) {
        makeVertex(vec3(1.0f, 0.0f, 0.0f), vec2(0.5f, 2.0f/3.0f)); //1
        makeVertex(vec3(1.0f, 0.0f, 1.0f), vec2(0.5f, 1.0f)); //2
        makeVertex(vec3(0.0f, 0.0f, 0.0f), vec2(0.25f, 2.0f/3.0f)); //0
        makeVertex(vec3(0.0f, 0.0f, 1.0f), vec2(0.25f, 1.0f)); //3
        EmitPrimitive();
    }
    if((aFacesMask & BLOCK_Z_POS) == BLOCK_Z_POS) {
        makeVertex(vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 2.0f/3.0f)); //3
        makeVertex(vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f/3.0f)); //7
        makeVertex(vec3(1.0f, 0.0f, 1.0f), vec2(0.75f, 2.0f/3.0f)); //2
        makeVertex(vec3(1.0f, 1.0f, 1.0f), vec2(0.75f, 1.0f/3.0f)); //6
        EmitPrimitive();
    }
    if((aFacesMask & BLOCK_Z_NEG) == BLOCK_Z_NEG) {
        makeVertex(vec3(1.0f, 0.0f, 0.0f), vec2(0.5f, 2.0f/3.0f)); //1
        makeVertex(vec3(1.0f, 1.0f, 0.0f), vec2(0.5f, 1.0f/3.0f)); //5
        makeVertex(vec3(0.0f, 0.0f, 0.0f), vec2(0.25f, 2.0f/3.0f)); //0
        makeVertex(vec3(0.0f, 1.0f, 0.0f), vec2(0.25f, 1.0f/3.0f)); //4
        EmitPrimitive();
    }

}

void makeVertex(vec3 pos, vec2 texCoord) {
    vTextCoords = texCoord;
    vNormal = normalize(cross(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz));
    gl_Position = gl_in[0].gl_Position + vec4(pos, 0.0);
    EmitVertex();
}
