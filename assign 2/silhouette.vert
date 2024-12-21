#version 140
#extension GL_ARB_compatibility: enable

in vec3 vPosition;
in vec3 vNormal;

uniform float m_Scale;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

uniform float thick;

void main() {
    
    vec3 P_obj = vPosition;
	P_obj.x = P_obj.x * m_Scale;
	P_obj.y = P_obj.y * m_Scale;
	P_obj.z = P_obj.z * m_Scale;

    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
	vec3 norm = normalize(normalMatrix * (vNormal / m_Scale));
    
    vec3 silhouettePosition = P_obj + (norm * thick);

    gl_Position = projectionMatrix * modelViewMatrix * vec4(silhouettePosition, 1.0);
}