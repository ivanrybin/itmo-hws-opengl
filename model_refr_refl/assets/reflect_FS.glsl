#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform float refraction;

uniform float REFL_WEIGHT;
uniform float REFR_WEIGHT;

uniform vec3 cameraPos;
uniform bool IS_COLOR;

uniform sampler2D texture_diffuse;
uniform samplerCube skybox;

void main() {
    float ratio = 1.0f / refraction;

    vec3 I = normalize(Position - cameraPos);

    vec4 REFL = texture(skybox, reflect(I, normalize(Normal)));
    vec4 REFR = texture(skybox, refract(I, normalize(Normal), ratio));
    vec4 COLR = texture(texture_diffuse, TexCoords);

    float COLR_WEIGHT = 1.0f - REFL_WEIGHT - REFR_WEIGHT;
    if (COLR_WEIGHT < 0) {
        COLR_WEIGHT = 0;
    }

    FragColor = REFL_WEIGHT * REFL + REFR_WEIGHT * REFR + COLR_WEIGHT * COLR;
}

