#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 textureCoords;

uniform vec4 plane;

out vec2 textureCOORDS;

void main() {
    gl_Position = vec4(vertexPosition, 1.0);
    textureCOORDS = textureCoords;
}