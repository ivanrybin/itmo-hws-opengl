#version 330 core
in vec2 textureCoords;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

out vec4 fragmentColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3  lightColor;
uniform vec3  lightPos;

void main() {

    // light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientStrength * lightColor;

    vec4 color = texture(texture_diffuse, textureCoords);
    color.xyz = (ambient + diffuse) * color.xyz;

    fragmentColor = color;
}