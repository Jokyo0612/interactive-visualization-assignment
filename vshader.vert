#version 140
#extension GL_ARB_compatibility : enable

in vec3 position;
out vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_FrontColor = gl_Color;
    color = gl_Color;
    gl_Position = projection * view * model * vec4(position, 1.0);     // clip coordination
}