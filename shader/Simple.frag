#version 120

in float y;

void main() {
    gl_FragColor = vec4(abs(y / 256.0f), 0.0, 0.6, 1.0);
}
