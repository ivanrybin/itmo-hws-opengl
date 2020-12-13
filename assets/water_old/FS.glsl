#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    vec3 I = normalize(cameraPos - Position);
    FragColor = textureCube(skybox, I);
}

