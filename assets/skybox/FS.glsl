#version 330 core
in vec3 Normal;
in vec3 textureCoords;

uniform samplerCube skybox;

out vec4 fragmentColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3  lightColor;

void main() {
    vec4 color = texture(skybox, textureCoords);
    color.xyz = (color.xyz * ambientStrength) * lightColor;

    fragmentColor = color;
    //    gl_FragColor.rgb = textureCube(skybox, Normal).rgb;
}