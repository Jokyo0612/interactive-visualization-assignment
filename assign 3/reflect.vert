#version 140
#extension GL_ARB_compatibility : enable

in vec3 vPosition;
in vec3 vNormal;

out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 afterChange;

void main() {
    WorldPos = vec3(model * vec4(vPosition, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * vNormal);

    mat4 model2 = model * afterChange;
	gl_Position = proj * view * model2 * vec4(vPosition, 1.0);
}