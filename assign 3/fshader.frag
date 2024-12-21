#version 140
#extension GL_ARB_compatibility: enable

in  vec4 color;
out vec4 fColor;

in vec2 texCoord;
uniform sampler2D texSampler;

void main() 
{ 
    fColor = color * texture2D(texSampler, texCoord);
} 
