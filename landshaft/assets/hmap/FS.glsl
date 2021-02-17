#version 330 core
in vec3 fragPos;
in vec3 normal;
in vec2 textureCOORDS;

uniform int repeats;
uniform sampler2D worldTexture;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3  lightColor;
uniform vec3  lightPos;

out vec4 fragmentColor;

void main() {
    vec2 repeatCoords = textureCOORDS.xy * repeats - floor(textureCOORDS.xy * repeats);

    // light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientStrength * lightColor;

    vec4 color = texture(worldTexture, repeatCoords);
    color.xyz = (ambient + diffuse) * color.xyz;

    fragmentColor = color;
}
