#version 140
#extension GL_ARB_compatibility: enable

uniform vec3 direct;
uniform vec4 base;

uniform int numShades;

in  vec3 normal;
in  vec4 color;
in  vec3 p;

out vec4 fColor;

void main() {
    // ����ȭ
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(direct);

    // ������ ���� ���� ���
    float dotProduct = max(dot(normal, lightDir), 0.0);

    // ������ ���� ���� ���� ����
    float step = 1.0 / float(numShades);
    float value = dotProduct / step;

    // ������ ���� ���� ����
    float shade = floor(value);
    float colorValue = shade * step;

    // ���� ���� ���
    fColor = vec4(base.rgb * colorValue, base.a); // ���� ����
}
