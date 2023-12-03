#version 330 core

layout(location = 0) in vec3 objPos;
layout(location = 1) in vec3 objPosNorm;


out vec3 worldPos;
out vec3 worldPosNorm;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 inverseTransposeMatrix;


void main() {
    worldPos = vec3(modelMatrix * vec4(objPos, 1.0));
    worldPosNorm = normalize(mat3(inverseTransposeMatrix) * objPosNorm);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(objPos, 1.0);

}
