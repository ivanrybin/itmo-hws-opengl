#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec4 clipPlane;
uniform bool isClipPlane;

out vec2 textureCoords;
out vec3 normal;
out vec3 fragPos;

void main() {
    gl_ClipDistance[0] = dot(vec4(vertexPosition, 1.0f), clipPlane);

    gl_Position = P * V * M * vec4(vertexPosition, 1.0);

    normal = vertexNormal;
    textureCoords = vertexTexCoord;
    fragPos = vec3(M * vec4(vertexPosition, 1.0f));
}
