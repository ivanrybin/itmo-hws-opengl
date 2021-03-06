#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Normal = mat3(transpose(inverse(model))) * vertexNormal;
    Position = vec3(model * vec4(vertexPosition, 1.0));
    TexCoords = vertexTexCoord;
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
