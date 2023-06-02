#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{
    vec4 sampled = texture(text, TexCoords);
    color = vec4(1.0f, 1.0f, 1.0f, sampled.r) * textColor;
}