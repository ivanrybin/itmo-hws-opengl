#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec4 clipSpace;

void main() {
    clipSpace =  P * V * M * vec4(vertexPosition, 1.0);
    gl_Position = clipSpace;
}
