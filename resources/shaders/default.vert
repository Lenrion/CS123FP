#version 330 core

layout(location = 0) in vec3 objPos;
layout(location = 1) in vec3 objPosNorm;


out vec3 worldPos;
out vec3 worldPosNorm;
out vec2 reflectUVCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 inverseTransposeMatrix;


void main() {
    worldPos = vec3(modelMatrix * vec4(objPos, 1.0));
    worldPosNorm = normalize(mat3(inverseTransposeMatrix) * objPosNorm);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(objPos, 1.0);

    // project world-space position onto the reflection texture already done in flipping camera in realtine
//    vec3 projPos = worldPos;


//    float u = 0.5f * projPos.x / projPos.z + 0.5f;
//    float v = 0.5f * projPos.y / projPos.z + 0.5f;

    // set the texture coordinates for the vertex
//    reflectUVCoords = worldPos.xz * 0.5 + 0.5;
//    vec3 viewDir = normalize(worldPos - vec3(inverse(viewMatrix)[3]));
//    vec3 reflectDir = reflect(viewDir, worldPosNorm);

//    vec3 up = vec3(0, 1, 0);
//    vec3 right = cross(up, viewDir);

//    float u = dot(reflectDir, right) * 2 + 0.5;
//    float v = dot(reflectDir, up) * 2 + 0.5;

//    reflectUVCoords = vec2(u, v);


    reflectUVCoords = vec2(worldPos.x*0.3, worldPos.z*0.3);

}
