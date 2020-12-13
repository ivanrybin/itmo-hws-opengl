#version 330 core
in vec4 clipSpace;

uniform sampler2D reflectionText;
uniform sampler2D refractionText;

out vec4 fragColor;

void main() {
    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);

    vec4 reflectColor = texture(reflectionText, reflectTexCoords);
    vec4 refractColor = texture(refractionText, refractTexCoords);

    fragColor = mix(reflectColor, refractColor, 0.42);
}
