#version 330 core

#define BLOCK_X_POS 1
#define BLOCK_X_NEG 2
#define BLOCK_Y_POS 4
#define BLOCK_Y_NEG 8
#define BLOCK_Z_POS 16
#define BLOCK_Z_NEG 32

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

layout (location = 1) in ivec2 aAtlasCoords[];
layout (location = 2) in int aFacesMask[];

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform ivec2 atlasSize;

out vec2 vTextCoords;
out vec3 vNormal;
out vec2 vAtlasCoords;

void makeFace(vec3 pos[4], vec2 textCoord[4]);
void makeVertex(vec3 pos, vec2 texCoord);

void main() {

    //aFacesMask = BLOCK_Y_POS | BLOCK_Y_NEG | BLOCK_X_POS | BLOCK_X_NEG | BLOCK_Z_POS | BLOCK_Z_NEG;
    int faces = BLOCK_Y_POS | BLOCK_Y_NEG | BLOCK_X_POS | BLOCK_X_NEG | BLOCK_Z_POS | BLOCK_Z_NEG;

    if((faces & BLOCK_X_POS) == BLOCK_X_POS) {
        vec3 ps[4];
        ps[0] = vec3(1.0f, 0.0f, 1.0f);
        ps[1] = vec3(1.0f, 1.0f, 1.0f);
        ps[2] = vec3(1.0f, 0.0f, 0.0f);
        ps[3] = vec3(1.0f, 1.0f, 0.0f);

        vec2 ts[4];
        ts[0] = vec2(0.75f, 2.0f/3.0f);
        ts[1] = vec2(0.75f, 1.0f/3.0f);
        ts[2] = vec2(0.5f, 2.0f/3.0f);
        ts[3] = vec2(0.5f, 1.0f/3.0f);

        makeFace(ps, ts);
    }
    if((faces & BLOCK_X_NEG) == BLOCK_X_NEG) {
        vec3 ps[4];
        ps[0] = vec3(0.0f, 0.0f, 0.0f);
        ps[1] = vec3(0.0f, 1.0f, 0.0f);
        ps[2] = vec3(0.0f, 0.0f, 1.0f);
        ps[3] = vec3(0.0f, 1.0f, 1.0f);

        vec2 ts[4];
        ts[0] = vec2(0.25f, 2.0f/3.0f);
        ts[1] = vec2(0.25f, 1.0f/3.0f);
        ts[2] = vec2(0.0f, 2.0f/3.0f);
        ts[3] = vec2(0.0f, 1.0f/3.0f);
        makeFace(ps, ts);
    }
    if((faces & BLOCK_Y_POS) == BLOCK_Y_POS) {
        vec3 ps[4];
        ps[0] = vec3(0.0f, 1.0f, 1.0f);
        ps[0] = vec3(0.0f, 1.0f, 0.0f);
        ps[0] = vec3(1.0f, 1.0f, 1.0f);
        ps[0] = vec3(1.0f, 1.0f, 0.0f);
        vec2 ts[4];
        ts[0] = vec2(0.25f, 0.0f);
        ts[1] = vec2(0.25f, 1.0f/3.0f);
        ts[2] = vec2(0.5f, 0.0f);
        ts[3] = vec2(0.5f, 1.0f/3.0f);
        makeFace(ps, ts);
    }
    if((faces & BLOCK_Y_NEG) == BLOCK_Y_NEG) {
        vec3 ps[4];
        ps[0] = vec3(1.0f, 0.0f, 0.0f);
        ps[0] = vec3(1.0f, 0.0f, 1.0f);
        ps[0] = vec3(0.0f, 0.0f, 0.0f);
        ps[0] = vec3(0.0f, 0.0f, 1.0f);
        vec2 ts[4];
        ts[0] = vec2(0.5f, 2.0f/3.0f);
        ts[1] = vec2(0.5f, 1.0f);
        ts[2] = vec2(0.25f, 2.0f/3.0f);
        ts[3] = vec2(0.25f, 1.0f);
        makeFace(ps, ts);
    }
    if((faces & BLOCK_Z_POS) == BLOCK_Z_POS) {
        vec3 ps[4];
        ps[0] = vec3(0.0f, 0.0f, 1.0f);
        ps[0] = vec3(0.0f, 1.0f, 1.0f);
        ps[0] = vec3(1.0f, 0.0f, 1.0f);
        ps[0] = vec3(1.0f, 1.0f, 1.0f);
        vec2 ts[4];
        ts[0] = vec2(1.0f, 2.0f/3.0f);
        ts[1] = vec2(1.0f, 1.0f/3.0f);
        ts[2] = vec2(0.75f, 2.0f/3.0f);
        ts[3] = vec2(0.75f, 1.0f/3.0f);
        makeFace(ps, ts);
    }
    if((faces & BLOCK_Z_NEG) == BLOCK_Z_NEG) {
        vec3 ps[4];
        ps[0] = vec3(1.0f, 0.0f, 0.0f);
        ps[0] = vec3(1.0f, 1.0f, 0.0f);
        ps[0] = vec3(0.0f, 0.0f, 0.0f);
        ps[0] = vec3(0.0f, 1.0f, 0.0f);
        vec2 ts[4];
        ts[0] = vec2(0.5f, 2.0f/3.0f);
        ts[1] = vec2(0.5f, 1.0f/3.0f);
        ts[2] = vec2(0.25f, 2.0f/3.0f);
        ts[3] = vec2(0.25f, 1.0f/3.0f);
        makeFace(ps, ts);
    }

}

void makeFace(vec3 pos[4], vec2 textCoord[4]) {
    vNormal = normalize(cross(pos[0] - pos[1], pos[0] - pos[2]));
    makeVertex(pos[0], textCoord[0]);
    makeVertex(pos[1], textCoord[1]);
    makeVertex(pos[2], textCoord[2]);
    makeVertex(pos[3], textCoord[3]);
    EndPrimitive();
}

void makeVertex(vec3 pos, vec2 texCoord) {
    vTextCoords = texCoord;
    vAtlasCoords = aAtlasCoords[0];

    gl_Position = projection * view * model * vec4(gl_in[0].gl_Position.xyz + pos, 1.0);
    EmitVertex();
}
