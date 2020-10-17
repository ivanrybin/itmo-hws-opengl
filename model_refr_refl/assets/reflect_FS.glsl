#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform float refraction;

uniform float REFL_WEIGHT;
uniform float REFR_WEIGHT;

uniform vec3 cameraPos;

uniform sampler2D texture_diffuse;
uniform samplerCube skybox;

uniform float n1; // world
uniform float n2; // model

uniform float color_percent;

void main() {
    //https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf

    float ratio = 1.0f / refraction;

    vec3 N = normalize(Normal);
    vec3 I = normalize(Position - cameraPos);

    float cos_i  = dot(-I, N);
    float sin2_t = pow(n1, 2) / pow(n2, 2) * (1 - pow(cos_i, 2));
    float cos_t  = pow(1 - sin2_t, 0.5);
    float R0 = pow((n1 - n2) / (n1 + n2), 2);
    bool TIR = (sin2_t > 1);

    float R_shlick2 = 0.0f;
    if (n1 <= n2) {
        R_shlick2 = R0 + (1 - R0) * pow(1 - cos_i, 5);
    } else if (n1 > n2 && !TIR) {
        R_shlick2 = R0 + (1 - R0) * pow(1 - cos_t, 5);
    } else if (n1 > n2 && TIR) {
        R_shlick2 = 1.0f;
    }

    float T = 1 - R_shlick2;

    vec4 REFL = vec4(texture(skybox, reflect(I, N)).rgb, 1.0f);
    vec4 REFR = vec4(texture(skybox, refract(I, N, n1 / n2)).rgb, 1.0f);
    vec4 COLR = vec4(texture(texture_diffuse, TexCoords).rgb, 1.0f);

    FragColor = mix(R_shlick2 * REFL + T * REFR, COLR, color_percent);
}

