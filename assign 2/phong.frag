#version 140
#extension GL_ARB_compatibility: enable

#define MAX 10

uniform int numLights;
uniform vec3 lightpos[MAX];
uniform float Kd, Ka, Ks;
uniform vec3 Ld[MAX], La[MAX], Ls[MAX];

uniform float alpha;
uniform vec3 attenuation;

in  vec3 normal;
in  vec4 color;
in  vec3 p;

out vec4 fColor;

void main() 
{ 
    vec3 finalColor = vec3(0.0);

    vec3 n = normalize(normal);
    vec3 v = normalize(-p);
    
    for (int i = 0; i < numLights + 1; i++) {
        
        vec3 l = normalize(lightpos[i] - p);
        vec3 r = normalize(reflect(-l, n));

        float dist = length(lightpos[i] - p);
        float att = 1.0 / (attenuation.x + attenuation.y * dist + attenuation.z * dist * dist);
        
        vec3 ambient = Ka * La[i];
        vec3 diffuse = max(dot(l, n), 0) * Kd * Ld[i] * color.rgb;
        vec3 specular = Ks * pow(max(dot(r, v), 0.0), alpha) * Ls[i];

        finalColor += ambient + (diffuse + specular) * att;
    }

    fColor = vec4(finalColor, 1.0);
} 