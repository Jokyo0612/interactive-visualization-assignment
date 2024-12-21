#version 140
#extension GL_ARB_compatibility: enable

in vec3 WorldPos;
in vec3 Normal;

out vec4 FragColor;

uniform samplerCube texSampler;
uniform vec3 cameraPos;

void main() {
    vec3 I = normalize(WorldPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));

    FragColor = vec4(texture(texSampler, R).rgb, 1.0);

}