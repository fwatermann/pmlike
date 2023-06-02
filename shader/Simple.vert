#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out float y;

void main() {
    vec4 pos = model * vec4(aPosition, 1.0f);
    y = pos.y;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}