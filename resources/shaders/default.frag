#version 330 core

in vec3 worldPos;
in vec3 worldPosNorm;
in vec3 inColor;
out vec4 fragColor;
uniform vec4 cameraWorldPos;

void main() {
    vec4 norm = vec4(normalize(worldPosNorm), 0);
    vec4 directionToCammy = normalize(cameraWorldPos - vec4(worldPos, 1.0));
    vec4 lightDir = vec4(-3.0, -2.0, -1.0, 0.0);
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec3 cSpecular = vec3(0.4, 0.3, 0.1);
    vec4 dirFromLight = normalize(-lightDir);
    float shininess = 6;

    float diffuseDot = max(0, min(1, dot(dirFromLight, norm)));
    fragColor += diffuseDot*lightColor*vec4(inColor, 1.0)*0.5;
    vec4 reflectedIncidentRay = normalize(reflect(normalize(lightDir), vec4(worldPosNorm, 0)));
    float speccyDot = max(0, min(1, dot(reflectedIncidentRay, directionToCammy)));
    float totalSpeccy = 0;
        if(shininess == 0){
            totalSpeccy = 1;
        } else {
            totalSpeccy = pow(speccyDot, shininess);
        }

    fragColor += totalSpeccy * 0.5 * vec4(cSpecular, 1.0) * lightColor;
    fragColor = vec4(inColor, 1.f);
}
