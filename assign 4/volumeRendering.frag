#version 140
#extension GL_ARB_compatibility: enable

in vec3 pixelPosition;
uniform vec3 eyePosition;
uniform vec3 objectMin;
uniform vec3 objectMax;
uniform vec3 up;
uniform sampler3D tex;
uniform sampler1D transferFunction;

uniform int mode;
uniform float isovalue;

float makeSample(vec3 currentPos)
{
    vec3 normalizedPos = (currentPos - objectMin) / (objectMax - objectMin);
    float sampleValue = texture(tex, normalizedPos).r;
    return sampleValue;
}

void main()
{	
	vec3 rayDir = normalize(pixelPosition - eyePosition);
    
    // start & end point carculation
    vec3 vMin = (objectMin - eyePosition) / rayDir;
    vec3 vMax = (objectMax - eyePosition) / rayDir;
    vec3 close = min(vMin, vMax);
    vec3 far = max(vMin, vMax);
    float enter = max(close.x, max(close.y, close.z));
    float exit = min(far.x, min(far.y, far.z));

    if (enter > exit || exit < 0.0) {
        discard;
    }

    vec3 start = eyePosition + enter * rayDir;
    vec3 end = eyePosition + exit * rayDir;
    
    float stepSize = 1.0 / 256.0;
    vec3 step = (end - start) * stepSize;

    vec3 currentPos = start;
    vec4 composedColor = vec4(0,0,0,0);

    // isosurface variables
    float maxValue = 0.0;
    float minValue = 999.0;
    float threshold = 0.4;
    bool find = false;

    for (int i = 0; i < 256; i++) {

        float sampleValue = makeSample(currentPos);

        if (mode == 1) {
            if (sampleValue > maxValue) {
                maxValue = sampleValue;
                composedColor = texture(transferFunction, maxValue);}
        }

        if (mode == 2) {
            
            float diff = abs(sampleValue - isovalue);

            if (diff < threshold && diff < minValue){
                minValue = diff;
                maxValue = sampleValue;
                composedColor = texture(transferFunction, maxValue);
                find = true;
            }

            if (find == true) {
                break;
            }
            
        }

        if (mode == 0) {
            vec4 color = texture(transferFunction, sampleValue);

            float adjAlpha = pow(color.a, 2.0);
            composedColor.rgb += (1.0 - composedColor.a) * color.rgb * adjAlpha;
            composedColor.a += (1.0 - composedColor.a) * adjAlpha;

            if (composedColor.a > 0.98) break;
        }

        currentPos += step;
    }

    if (mode == 2 && find == true) {

        float Ka = 0.3;
        float Kd = 0.7;
        float Ks = 0.5;

        vec3 La = vec3(0.4, 0.4, 0.4);
        vec3 Ld = vec3(1.0, 1.0, 1.0);
        vec3 Ls = vec3(1.0, 1.0, 1.0);

        vec3 attenuation = vec3(1.0, 0.1, 0.01);
    
        vec3 upx = vec3(currentPos.x + stepSize, currentPos.y, currentPos.z);
        vec3 dnx = vec3(currentPos.x - stepSize, currentPos.y, currentPos.z);
        vec3 upy = vec3(currentPos.x, currentPos.y + stepSize, currentPos.z);
        vec3 dny = vec3(currentPos.x, currentPos.y - stepSize, currentPos.z);
        vec3 upz = vec3(currentPos.x, currentPos.y, currentPos.z + stepSize);
        vec3 dnz = vec3(currentPos.x, currentPos.y, currentPos.z - stepSize);
        float gx = (makeSample(upx) - makeSample(dnx)) / (2 * stepSize);
        float gy = (makeSample(upy) - makeSample(dny)) / (2 * stepSize);
        float gz = (makeSample(upz) - makeSample(dnz)) / (2 * stepSize);

        vec3 n = normalize(vec3(gx, gy, gz));
        vec3 v = normalize(-currentPos);
        vec3 l = normalize(vec3(0.0, 10.0, 0.0) - currentPos);
        vec3 r = normalize(reflect(-l, n));

        float dist = length(vec3(0.0, 10.0, 0.0) - currentPos);
        float att = 1.0 / (attenuation.x + attenuation.y * dist + attenuation.z * dist * dist);
        
        vec3 ambient = Ka * La;
        vec3 diffuse = max(dot(l, n), 0) * Kd * Ld * composedColor.rgb;
        vec3 specular = Ks * pow(max(dot(r, v), 0.0), 32) * Ls;

        composedColor = vec4(ambient + (diffuse + specular) * att, 1.0);

    }

    gl_FragColor = composedColor;

}