#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec4 objPos;
in vec3 objPosNormal;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;



void main() {


        fragColor = vec4(0.0, 0.5, 1.0, 0.5);

}

