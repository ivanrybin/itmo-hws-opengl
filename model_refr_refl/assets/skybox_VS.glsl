#version 330 core
layout (location = 0) in vec3 vertexPosition;

out vec3 textureCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    textureCoords = vertexPosition;
    vec4 pos = projection * view * vec4(vertexPosition, 1.0);
//    gl_Position = pos.xyww;
    gl_Position = pos;
}