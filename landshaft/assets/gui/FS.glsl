#version 330 core
in vec2 textureCOORDS;

uniform sampler2D worldTexture;

out vec4 fragmentColor;

void main() {
     fragmentColor = texture(worldTexture, vec2(textureCOORDS.x, -textureCOORDS.y));
//    fragmentColor = vec4(0.1, 0.5, 1.0, 1.0);
}