#version 330 core
in vec4 clipSpace;
in vec2 textCoords;

uniform sampler2D reflectionText;
uniform sampler2D refractionText;
uniform sampler2D dudv_water;

uniform float moveWaves;

out vec4 fragColor;

const float distorStrength = 0.01;

// https://www.youtube.com/playlist?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh

void main() {
    vec2 distort1 = (texture(dudv_water, vec2(textCoords.x + moveWaves, textCoords.y)).rg * 2.0 - 1.0) * distorStrength;
    vec2 distort2 = (texture(dudv_water, vec2(-textCoords.x + moveWaves, textCoords.y + moveWaves)).rg * 2.0 - 1.0) * distorStrength;

    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y) + distort1 + distort2;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y) + distort1 + distort2;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

    vec4 reflectColor = texture(reflectionText, reflectTexCoords);
    vec4 refractColor = texture(refractionText, refractTexCoords);

    fragColor = mix(reflectColor, refractColor, 0.42);
}
