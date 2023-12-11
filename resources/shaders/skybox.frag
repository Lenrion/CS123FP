#version 330 core

in vec3 TextureCoords;

out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
    fragColor =  texture(skybox, normalize(TextureCoords));//vec4(0.0, 0.0, 1.0, 0.5); //
}
