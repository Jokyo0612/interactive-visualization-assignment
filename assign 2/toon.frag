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
    // 정규화
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(direct);

    // 내적을 통한 각도 계산
    float dotProduct = max(dot(normal, lightDir), 0.0);

    // 각도에 따른 색상 구간 설정
    float step = 1.0 / float(numShades);
    float value = dotProduct / step;

    // 구간에 따라 색상 결정
    float shade = floor(value);
    float colorValue = shade * step;

    // 최종 색상 계산
    fColor = vec4(base.rgb * colorValue, base.a); // 색상 조정
}
