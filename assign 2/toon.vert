#version 140
#extension GL_ARB_compatibility : enable

in vec3 vPosition;
in vec3 vNormal;

uniform float m_Scale;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec4 color;
out vec3 normal;

out vec3 p;

void main(void)
{	
	vec4 P_obj = vec4(vPosition, 1.0);
	P_obj.x = P_obj.x * m_Scale;
	P_obj.y = P_obj.y * m_Scale;
	P_obj.z = P_obj.z * m_Scale;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
	gl_Position = projectionMatrix * modelViewMatrix * P_obj;
	normal = normalize(normalMatrix * (vNormal/m_Scale));

	p = vec3(modelViewMatrix * P_obj);

	color = gl_Color;
}