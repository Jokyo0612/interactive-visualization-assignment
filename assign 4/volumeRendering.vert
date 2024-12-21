#version 140
#extension GL_ARB_compatibility: enable

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 pixelPosition;

void main(){
    pixelPosition= vec3(gl_Vertex);
    gl_Position = proj * view * model * gl_Vertex;
}