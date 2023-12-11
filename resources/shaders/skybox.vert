#version 330 core

layout(location = 0) in vec3 objPos;
layout(location = 1) in vec3 objPosNorm;

out vec3 TextureCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPos;

void main() {
    vec3 worldPos = vec3(modelMatrix * vec4(objPos, 1.0));

    //direction vector from the center of the skybox to the vertex
    vec3 directionVector = normalize(worldPos - cameraPos);

    //  direction vector as the texture coordinates to the fragment shader
    TextureCoords = directionVector;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(objPos, 1.0);
}


