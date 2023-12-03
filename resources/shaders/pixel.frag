#version 330 core
in vec2 uv;
uniform sampler2D tex;
uniform bool invert;
uniform bool grayscale;
uniform bool sharpen;
uniform bool blur;
uniform int width;
uniform int height;

out vec4 fragColor;

void main()
{

    fragColor = vec4(0.f,0.f,0.f,1.f);

    if(sharpen){
        mat3 sharpenKernel = (1.f/9.f) * mat3(-1.f,   -1.f, -1.f,
                                          -1.f,   17.f, -1.f,
                                          -1.f,   -1.f, -1.f);


        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                float uOffset = ((i - 1.f)/width);
                float vOffset = (((j - 1.f))/height);
                vec2 newUV = vec2(uOffset + uv[0], vOffset + uv[1]);

                fragColor += sharpenKernel[i][j] * texture(tex, newUV);

            }
        }
    }
    if(blur){
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 5; j++){
                float uOffset = ((i - 1.f)/width);
                float vOffset = (((j - 1.f))/height);
                vec2 newUV = vec2(uOffset + uv[0], vOffset + uv[1]);

                fragColor += 0.04f * texture(tex, newUV);

            }
        }
    }

    if(!sharpen && !blur){
        fragColor = texture(tex, uv);
    }

    if(invert){
        fragColor = vec4(1) - fragColor;
//        fragColor[3] = 1;
    }
    if(grayscale){
        fragColor = vec4( 0.299 * fragColor[0] + (0.587 * fragColor[1]) + (0.114 * fragColor[2]));
//        fragColor[3] = 1;
    }
    fragColor[3] = 1.f;


}
