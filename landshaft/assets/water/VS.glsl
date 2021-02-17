#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec4 clipSpace;
out vec2 textCoords;

const float tiling = 100;

void main() {
    clipSpace =  P * V * M * vec4(vertexPosition, 1.0);
    gl_Position = clipSpace;

    textCoords = vec2(vertexPosition.x / 2, vertexPosition.z / 2) * tiling;
}
