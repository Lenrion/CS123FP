#version 330 core

in vec3 worldPos;
in vec3 worldPosNorm;
out vec4 fragColor;
//uniform float k_a;
//uniform float k_d;
//uniform float k_s;
//uniform float shininess;
uniform vec4 cameraWorldPos;
//uniform vec4 ambient;
//uniform vec4 diffuse;
//uniform vec4 specular;
in vec3 inColor;

struct Light
{
  vec4 pos;
  vec4 dir;
  vec4 color;
  int type;
  float a;
  float b;
  float c;
  float angle;
  float penumbra;
};

//uniform Light lights[8];
//uniform int numLights;





void main() {
//    float fatt;
//    //Ambient
//    fragColor = k_a * ambient;
//    for(int i = 0; i < numLights; i++){
//        float d = sqrt(
//                    (worldPos[0] - lights[i].pos[0]) * (worldPos[0] - lights[i].pos[0]) +
//                    (worldPos[1] - lights[i].pos[1]) * (worldPos[1] - lights[i].pos[1]) +
//                    (worldPos[2] - lights[i].pos[2]) * (worldPos[2] - lights[i].pos[2]));
//        vec4 directionToLight;
//        switch(lights[i].type){
//            case 0: // Directional Lights
//                fatt = 1;
//                directionToLight = normalize(-lights[i].dir);
//                break;
//            case 1: //Point Lights
//                fatt = 1 / (lights[i].a + (lights[i].b *d) + (lights[i].c * d * d ));
//                directionToLight = normalize(lights[i].pos - vec4(worldPos,1.f));
//                break;
//            case 2: //Spot Lights
//                fatt = 1 / (lights[i].a + (lights[i].b *d) + (lights[i].c * d * d ));
//                directionToLight = normalize(lights[i].pos - vec4(worldPos,1.f));
//                float cosTheta = dot(-lights[i].dir, directionToLight);
//                float theta = acos(cosTheta);
//                float x = theta;
//                float innerTheta = lights[i].angle - lights[i].penumbra;
//                float outerTheta = lights[i].angle;
//                float fallout = -2.f * pow((x - innerTheta )/(outerTheta - innerTheta), 3.f) + 3.f * pow((x - innerTheta )/(outerTheta - innerTheta), 2.f);

//                if(x > outerTheta){
//                    fatt = 0;
//                } else if(innerTheta < x && x <= outerTheta){
//                    fatt *= (1- fallout);
//                }
//                break;
//        }

//        //Diffuse
//        vec4 surfaceNormal = normalize(vec4(worldPosNorm,0.f));
//        float diffuseFactor= max(dot(surfaceNormal, directionToLight),0.f);
//        fragColor += (fatt * lights[i].color ) * (k_d * diffuse ) * diffuseFactor;

//        //Specular
//        vec4 reflectedLight = normalize(reflect(-directionToLight,surfaceNormal));
//        vec4 directionToCamera = normalize(cameraWorldPos - vec4(worldPos,1.f));

//        float specularFactor = 1.f;
//        float newAngle = min(max(dot(reflectedLight,directionToCamera),0),1);
//        if(shininess > 0){
//            specularFactor = pow(newAngle, shininess);
//        }

//        if(newAngle >= 0){
//            fragColor += (fatt * lights[i].color) * (k_s * specular) * (specularFactor);
//        }
//    }
    fragColor = vec4(inColor, 1.f);


}
