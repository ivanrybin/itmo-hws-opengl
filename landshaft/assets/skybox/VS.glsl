#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 textureCoords;
out vec3 Normal;

void main() {
    Normal = vertexNormal;
    textureCoords = vertexPosition;
    vec4 pos = P * V * vec4(vertexPosition, 1.0);
    gl_Position = pos.xyww;
//    gl_Position = P * M * V * vec4(vertexPosition, 1.0f);
}
