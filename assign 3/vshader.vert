#version 140
#extension GL_ARB_compatibility : enable

in vec3 vPosition;
in vec2 vTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 afterChange;

out vec4 color;
out vec2 texCoord;

void main(void)
{	
	mat4 model2 = model * afterChange;
	gl_Position = proj * view * model2 * vec4(vPosition, 1.0);
		
	texCoord = vTexture;
	color = gl_Color;
}