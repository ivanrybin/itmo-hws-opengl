#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;
out vec4 clipSpace;

void main() {
    Normal = mat3(transpose(inverse(M))) * vertexNormal;
    Position = vec3(M * vec4(vertexPosition, 1.0));
    TexCoords = vertexTexCoord;

    clipSpace =  P * V * M * vec4(vertexPosition, 1.0);
    gl_Position = clipSpace;
}
