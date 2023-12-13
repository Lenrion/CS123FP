#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 worldPos;
in vec3 worldPosNorm;
in vec2 reflectUVCoords;
uniform sampler2D reflectionTex;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;


uniform vec3 lightDir;
uniform float timer;


void main() {
    //fragColor = texture(reflectionTex, reflectUVCoords); //vec4(0.0, 0.5, 1.0, 0.5); /

    vec4 reflectionColor = texture(reflectionTex, reflectUVCoords);

    // Modify the y component of the world position for the wave effect
    vec3 worldPosWave = worldPos;
    worldPosWave.y += 0.01 *sin(worldPos.x + timer) + cos(worldPos.z + timer);

    // Calculate the normal
    vec3 normal = normalize(worldPosNorm + worldPosWave);

//    float blendFactor = normal.y * 0.5 + 0.5;
    // Use the y component of the normal as a blend factor
    //blendFactor = pow(blendFactor, 0.5);

    // Blend the reflection color with a base color
    float blendFactor = clamp(worldPosWave.y, 0.0, 1.0);
    vec4 baseColor = vec4(0.4, 0.5, 0.7, 1.f);
    vec4 final = mix(reflectionColor, baseColor, blendFactor); //vec4(worldPosWave,1.f);////vec4(worldPosWave.y,worldPosWave.y,worldPosWave.y,1.f);//mix(baseColor, reflectionColor, blendFactor);

//    // Add sparkles
//    float sparkle = rand(vec2(timer, worldPos.x + worldPos.z)) > 0.95 ? 1.0 : 0.0;
//    final += vec4(vec3(sparkle), 0.0);

    final.a = 0.75f;


    fragColor = final;
}

