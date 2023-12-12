#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "shapes/Cone.h"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Cube.h"
#include "shapes/Water.h"
#include "shapes/skybox.h"
#include "utils/shaderloader.h"
#include "camera.h"
#include "terraingenerator.h"
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    makeCurrent();

    // Task 19: Clean up your VBO and VAO memory here
    GLsizei size = 1;
    glDeleteBuffers(size, &sphereVBO);
    glDeleteVertexArrays(size, &sphereVAO);
    glDeleteBuffers(size, &coneVBO);
    glDeleteVertexArrays(size, &coneVAO);
    glDeleteBuffers(size, &cylinderVBO);
    glDeleteVertexArrays(size, &cylinderVAO);
    glDeleteBuffers(size, &cubeVBO);
    glDeleteVertexArrays(size, &cubeVAO);
    for(int i = 0; i < terrainRowMap.size(); i++){
        for(int j = 0; j < terrainMap.size(); j++){
        glDeleteBuffers(size, &terrainRowMap[i][j].terrainVBO);
        glDeleteVertexArrays(size, &terrainRowMap[i][j].terrainVAO);
        }
    }



    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    glDeleteProgram(shader);
    glDeleteProgram(m_filterShader);
    glDeleteProgram(m_terrainShader);
    doneCurrent();

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;


    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    m_filterShader = ShaderLoader::createShaderProgram(":/resources/shaders/pixel.vert", ":/resources/shaders/pixel.frag");
    shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_water_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",":/resources/shaders/water.frag");
    m_skybox_shader = ShaderLoader::createShaderProgram(":/resources/shaders/skybox.vert",":/resources/shaders/skybox.frag");
    m_terrainShader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain.vert", ":/resources/shaders/terrain.frag");

    // Students: anything requiring OpenGL calls when the program starts should be done here


    GLsizei size = 1;
    cone = Cone();
    cylinder = Cylinder();
    sphere = Sphere();
    cube = Cube();
    terrain = TerrainGenerator();
    skybox = Skybox();
//    m_tree = Tree(glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0.3f,0.3f,0.3f));
    createSphere();
    createCone();
    createCylinder();
    createCube();
    createSkybox();
    createTerrain(0,0);


    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.f,  1.f, 0.0f, 0.f, 1.f,
            -1.f, -1.f, 0.0f, 0.f, 0.f,
            1.f, -1.f, 0.0f, 1.f, 0.f,
            1.f,  1.f, 0.0f, 1.f, 1.f,
            -1.f,  1.f, 0.0f, 0.f, 1.f,
            1.f, -1.f, 0.0f, 1.f, 0.f
        };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();

    std::vector<std::string> faces
        {
            "../resources/images/xpos.png",
            "../resources/images/xneg.png",
            "../resources/images/ypos.png", //
            "../resources/images/yneg.png", //
            "../resources/images/zpos.png",
            "../resources/images/zneg.png"
        };

    m_skybox_texture = loadCubemap(faces);

}

GLuint Realtime::loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                         );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void Realtime::paintGL() {

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjectionMatrix();


    glUseProgram(shader);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Clear screen color and depth before painting
//    GLint cameraWorldPosLocation  = glGetUniformLocation(shader, "cameraWorldPos");
//    glUniform4fv(cameraWorldPosLocation,1.f,&(glm::inverse(m_view)*glm::vec4(0.f,0.f,0.f,1.f))[0]);

//    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
//    glUniformMatrix4fv(glGetUniformLocation(shader, "inverseTransposeMatrix"), 1, GL_FALSE, &glm::inverse(glm::transpose(m_tree.m_modelMatrix))[0][0]);    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
//    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &m_tree.m_modelMatrix[0][0]);
//    glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_FALSE, &m_proj[0][0]);

//    glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);
//    glBindVertexArray(m_tree.m_treeVao);
//    glDrawArrays(GL_TRIANGLES, 0, m_tree.generateShape().size() / 6);
//    glBindVertexArray(0);
//    glUseProgram(0);


//    GLint ksLocation = glGetUniformLocation(shader, "k_s");
//    glUniform1f(ksLocation, m_ks);

//    // Task 12: pass m_ka into the fragment shader as a uniform
//    GLint kaLocation = glGetUniformLocation(shader, "k_a");
//    glUniform1f(kaLocation, m_ka);

//    GLint kdLocation = glGetUniformLocation(shader, "k_d");
//    glUniform1f(kdLocation, m_kd);

    //The following chunk of code handles the skybox shader :)
    glEnable(GL_DEPTH_CLAMP);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);

    glDepthMask(GL_FALSE);
    glUseProgram(0);
    glUseProgram(m_skybox_shader);

    GLint skyboxUniform = glGetUniformLocation(m_skybox_shader, "skybox");
    glUniform1i(skyboxUniform, 0);
    //^texture binding and putting into skybox var in shader

    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, skyboxData.size() / 6);
    // Task 6: pass in m_model as a uniform into the shader program
    GLint modelMatrixLocation = glGetUniformLocation(m_skybox_shader, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_model[0][0]);//get first index's address

    GLint location = glGetUniformLocation(m_skybox_shader, "cameraPos");
    glUniform3fv(location, 1, &m_camera.getCameraPos()[0]);

    // Task 7: pass in m_view and m_proj
    GLint viewMatrixLocation = glGetUniformLocation(m_skybox_shader, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &m_view[0][0]);//get first index's address

    GLint projectionMatrixLocation = glGetUniformLocation(m_skybox_shader, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &m_proj[0][0]);//get first index's address

    GLint cameraPosLocation = glGetUniformLocation(m_skybox_shader, "worldSpaceCamera");
    glUniform4fv(cameraPosLocation, 1, &glm::inverse(m_view)[3][0]);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);


    for(RenderShapeData shape: renderData.shapes){

        m_model = shape.ctm;
        glm::mat4 mvMatrix = m_view * m_model;
        glUseProgram(m_terrainShader);
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "mvMatrix"), 1, GL_FALSE, &m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);
//        GLint ambientLocation = glGetUniformLocation(shader, "ambient");
//        glUniform4fv(ambientLocation,1.f, &shape.primitive.material.cAmbient[0]);

//        GLint diffuseLocation = glGetUniformLocation(shader, "diffuse");
//        glUniform4fv(diffuseLocation,1.f , &shape.primitive.material.cDiffuse[0]);

//        GLint specularLocation = glGetUniformLocation(shader, "specular");
//        glUniform4fv(specularLocation, 1.f, &shape.primitive.material.cSpecular[0]);

//        GLint shininessLocation = glGetUniformLocation(shader, "shininess");
//        glUniform1f(shininessLocation, shape.primitive.material.shininess);

//        m_model = shape.ctm;
//        GLint modelMatrixLocation = glGetUniformLocation(shader, "modelMatrix");
//        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_model[0][0]);
//        GLint inverseTransposeMatrixLocation = glGetUniformLocation(shader, "inverseTransposeMatrix");
//        glUniformMatrix4fv(inverseTransposeMatrixLocation, 1, GL_FALSE, &glm::inverse(glm::transpose(m_model))[0][0]);


//        switch(shape.primitive.type){
//            case PrimitiveType::PRIMITIVE_SPHERE:
//                glBindVertexArray(sphereVAO);
//                glDrawArrays(GL_TRIANGLES, 0, sphereData.size() / 6);
//                break;
//            case PrimitiveType::PRIMITIVE_CYLINDER:
//                glBindVertexArray(cylinderVAO);
//                glDrawArrays(GL_TRIANGLES, 0, cylinderData.size() / 6);
//                break;
//            case PrimitiveType::PRIMITIVE_CUBE:
//                glBindVertexArray(cubeVAO);
//                glDrawArrays(GL_TRIANGLES, 0, cubeData.size() / 3);
//                break;
//            case PrimitiveType::PRIMITIVE_CONE:
//                glBindVertexArray(coneVAO);
//                glDrawArrays(GL_TRIANGLES, 0, coneData.size() / 3);
//                break;
//            }
        glm::vec3 cameraPos = m_camera.getCameraPos();
        for(int i = 0; i < 25 ; i++){
            glUseProgram(m_terrainShader);
            int row = (i % 5) - 2 + cameraPos.z;
            int col = (i / 5) - 2 + cameraPos.x;
            glBindVertexArray(terrainRowMap[row][col].terrainVAO);
            int res = terrain.getResolution();
            glPolygonMode(GL_FRONT_AND_BACK,false);
            glDrawArrays(GL_TRIANGLES, 0, res * res * 6);
            glBindVertexArray(0);
            glUseProgram(0);
            paintTrees(terrainRowMap[row][col].trees);
            if(!terrainRowMap[row][col].pond.waterData.empty()){
                paintWater(terrainRowMap[row][col].pond);
                std::cout << "pond"<< std::endl;
            }
        }
        glUseProgram(0);




    }
    glUseProgram(0);

    glUseProgram(m_filterShader);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1i(glGetUniformLocation(m_filterShader,"invert"), settings.perPixelFilter);
    glUniform1i(glGetUniformLocation(m_filterShader,"grayscale"), settings.extraCredit1);
    glUniform1i(glGetUniformLocation(m_filterShader,"sharpen"), settings.kernelBasedFilter);
    glUniform1i(glGetUniformLocation(m_filterShader,"blur"), settings.extraCredit2);
    glUniform1i(glGetUniformLocation(m_filterShader,"width"), m_screen_width);
    glUniform1i(glGetUniformLocation(m_filterShader,"height"), m_screen_height);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_fbo_texture);
    glUniform1i(glGetUniformLocation(m_filterShader,"tex"), 0);


    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void Realtime::paintWater(TerrainGenerator::WaterInfo water){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(m_water_shader);
    glBindVertexArray(water.waterVAO);
    glUniformMatrix4fv(glGetUniformLocation(m_water_shader, "modelMatrix"), 1, GL_FALSE, &m_model[0][0]);//get first index's address
    glUniformMatrix4fv(glGetUniformLocation(m_water_shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);//get first index's address
    glUniformMatrix4fv(glGetUniformLocation(m_water_shader, "projectionMatrix"), 1, GL_FALSE, &m_proj[0][0]);//get first index's address
    glUniform1f(glGetUniformLocation(m_water_shader, "k_a"), m_ka);//get first index's address
    glUniform1f(glGetUniformLocation(m_water_shader, "k_d"), m_kd);
    glUniform1f(glGetUniformLocation(m_water_shader, "k_s"), m_ks);//get first index's address



    glm::vec4 cSpecular = glm::vec4(1.f);
    glm::vec4 cAmbient = glm::vec4(1.f);
    glm::vec4 cDiffuse = glm::vec4(0.f, 0.f, 1.f, 1.f);
    GLint cameraPosLocation1 = glGetUniformLocation(m_water_shader, "worldSpaceCamera");
    glUniform4fv(cameraPosLocation1, 1, &glm::inverse(m_view)[3][0]);
    //changed these three to not take in the json values of a shape.
    glUniform4fv(glGetUniformLocation(m_water_shader, "cSpecular"), 1, &cSpecular[0]);
    glUniform4fv(glGetUniformLocation(m_water_shader, "cAmbient"), 1, &cAmbient[0]);
    glUniform4fv(glGetUniformLocation(m_water_shader, "cDiffuse"), 1, &cDiffuse[0]);

    glDrawArrays(GL_TRIANGLES, 0, water.waterData.size() / 6);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void Realtime::paintTrees(std::vector<TerrainGenerator::TreeInfo> trees){
    glUseProgram(shader);
    std::cout<< trees.size() << std::endl;
    for(TerrainGenerator::TreeInfo tree: trees){
                GLint cameraWorldPosLocation  = glGetUniformLocation(shader, "cameraWorldPos");
                glUniform4fv(cameraWorldPosLocation,1.f,&(glm::inverse(m_view)*glm::vec4(0.f,0.f,0.f,1.f))[0]);
                glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shader, "inverseTransposeMatrix"), 1, GL_FALSE, &glm::inverse(glm::transpose(tree.modelMat))[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &tree.modelMat[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_FALSE, &m_proj[0][0]);
                glBindVertexArray(tree.treeVAO);
                glDrawArrays(GL_TRIANGLES, 0, tree.treeData.size() / 6);
                glBindVertexArray(0);

    }
    glUseProgram(0);

}


void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    makeFBO();
    m_proj = m_camera.getProjectionMatrix();


}

//Makes the FBO
void Realtime::makeFBO(){
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_fbo_width,m_fbo_height,0,GL_RGBA,GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}



void Realtime::sceneChanged() {
    if(shader > 0){

       SceneParser::parse(settings.sceneFilePath, renderData);
//        std::vector<SceneLightData> lights = renderData.lights;
//        int numLights = 0;
//       glUseProgram(shader);
//       for(int i = 0; i < lights.size(); i++){
//            //pos
//                int type;
//                switch(lights[i].type) {
//                case LightType::LIGHT_DIRECTIONAL:
//                    type = 0;
//                    break;
//                case LightType::LIGHT_POINT:
//                    type = 1;
//                    break;
//                case LightType::LIGHT_SPOT:
//                    type = 2;
//                    std::string lightAngleName = "lights[" + std::to_string(i) + "].angle";
//                    GLint lightAngleLoc = glGetUniformLocation(shader, lightAngleName.c_str());
//                    glUniform1f(lightAngleLoc,lights[i].angle);
//                    std::string lightPenumbraName = "lights[" + std::to_string(i) + "].penumbra";
//                    GLint lightPenumbraLoc = glGetUniformLocation(shader, lightPenumbraName.c_str());
//                    glUniform1f(lightPenumbraLoc,lights[i].penumbra);
//                    break;
//                }

//                std::string lightAName = "lights[" + std::to_string(i) + "].a";
//                GLint lightALoc = glGetUniformLocation(shader, lightAName.c_str());
//                glUniform1f(lightALoc,lights[i].function[0]);

//                std::string lightBName = "lights[" + std::to_string(i) + "].b";
//                GLint lightBLoc = glGetUniformLocation(shader, lightBName.c_str());
//                glUniform1f(lightBLoc,lights[i].function[1]);

//                std::string lightCName = "lights[" + std::to_string(i) + "].c";
//                GLint lightCLoc = glGetUniformLocation(shader, lightCName.c_str());
//                glUniform1f(lightCLoc,lights[i].function[2]);


//                std::string lightTypeName = "lights[" + std::to_string(i) + "].type";
//                GLint lightTypeLoc = glGetUniformLocation(shader, lightTypeName.c_str());
//                glUniform1i(lightTypeLoc,type);

//                std::string lightPosName = "lights[" + std::to_string(i) + "].pos";
//                GLint lightPosLoc = glGetUniformLocation(shader, lightPosName.c_str());
//                glUniform4fv(lightPosLoc,1.f, &renderData.lights[i].pos[0]);
//                //dir
//                std::string lightDirName = "lights[" + std::to_string(i) + "].dir";
//                GLint lightDirLoc = glGetUniformLocation(shader, lightDirName.c_str());
//                glUniform4fv(lightDirLoc,1.f, &renderData.lights[i].dir[0]);
//                //color
//                std::string lightColorName = "lights[" + std::to_string(i) + "].color";
//                GLint lightColorLoc = glGetUniformLocation(shader, lightColorName.c_str());
//                glUniform4fv(lightColorLoc, 1.f, &renderData.lights[i].color[0]);
//                numLights++;


//       }
//       glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);
//       glUseProgram(0);

       m_camera = Camera(renderData.cameraData, size().width(), size().height());
       m_model = glm::mat4(1.f);
       m_view = m_camera.getViewMatrix();
       m_proj = m_camera.getProjectionMatrix();
       m_ka = renderData.globalData.ka;
       m_ks = renderData.globalData.ks;
       m_kd = renderData.globalData.kd;

       update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::settingsChanged() {
    if(shader > 0){
       SceneParser::parse(settings.sceneFilePath, renderData);
       m_camera.update(renderData.cameraData, size().width(), size().height());
       m_view = m_camera.getViewMatrix();
       m_proj = m_camera.getProjectionMatrix();
       createSphere();
       createCone();
       createCylinder();
       createCube();


       update(); // asks for a PaintGL() call to occur
    }
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);


        m_camera.rotateCamera(deltaX, deltaY);
        update();
    }
}





void Realtime::timerEvent(QTimerEvent *event) {

    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    glm::vec3 movement(0.0f, 0.0f, 0.0f);
    float speed = 5.0f;

    if (m_keyMap[Qt::Key_W])  {
        movement += speed * m_camera.look;
    }
    if (m_keyMap[Qt::Key_S]) {
        movement -= speed * m_camera.look;
    }
    if (m_keyMap[Qt::Key_A])  {
        movement -= speed * glm::cross(m_camera.look, m_camera.up);
    }
    if (m_keyMap[Qt::Key_D]) {
        movement += speed * glm::cross(m_camera.look, m_camera.up);
    }
    if (m_keyMap[Qt::Key_Control])  {
        movement += speed * glm::vec3(0.f,-1.f,0.f);
    }
    if (m_keyMap[Qt::Key_Space]) {
        movement += speed * glm::vec3(0.f,1.f,0.f);
    }

    m_camera.updateCameraPos(movement * deltaTime);

    skybox.m_position = m_camera.getCameraPos();
    createSkybox();
    createTerrain(m_camera.getCameraPos().z,m_camera.getCameraPos().x);


    // Use deltaTime and m_keyMap here to move around
    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}

void Realtime::createSphere(){
    //Sphere
    sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    sphereData = sphere.generateShape();
    GLsizei size = 1;
    glGenBuffers(size,&sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereData.size() * sizeof(GLfloat),sphereData.data(), GL_STATIC_DRAW);
    glGenVertexArrays(size, &sphereVAO);
    glBindVertexArray(sphereVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 }

void Realtime::createSkybox(){
    skybox.updateParams(settings.shapeParameter1);
    skyboxData = skybox.generateShape();
    GLsizei size = 1;
    glGenBuffers(size,&skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxData.size() * sizeof(GLfloat),skyboxData.data(), GL_STATIC_DRAW);
    glGenVertexArrays(size, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::createCone(){
    cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    coneData = cone.generateShape();
    GLsizei size = 1;
    glGenBuffers(size,&coneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, coneVBO);

    glBufferData(GL_ARRAY_BUFFER, coneData.size() * sizeof(GLfloat),coneData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(size, &coneVAO);
    glBindVertexArray(coneVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Realtime::createCylinder(){
    cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cylinderData = cylinder.generateShape();
    GLsizei size = 1;
    glGenBuffers(size,&cylinderVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);

    glBufferData(GL_ARRAY_BUFFER, cylinderData.size() * sizeof(GLfloat),cylinderData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(size, &cylinderVAO);
    glBindVertexArray(cylinderVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Realtime::createCube(){
    cube.updateParams(settings.shapeParameter1);
    cubeData = cube.generateShape();
    GLsizei size = 1;
    glGenBuffers(size,&cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER, cubeData.size() * sizeof(GLfloat),cubeData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(size, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Realtime::createTerrain(float xOffset, float yOffset){
    for(int i = 0; i < 25; i++){
        int row = (i / 5) - 2 + xOffset;
        int col = (i % 5) - 2 + yOffset;
        if(terrainRowMap[row][col].terrainData.empty()){
            TerrainPatch info;
            GLuint terrainVBO;
            GLuint terrainVAO;
            std::vector<float> terrainData = terrain.generateTerrain(row,col);
            GLsizei size = 1;
            glGenBuffers(size,&terrainVBO);
            glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);

            glBufferData(GL_ARRAY_BUFFER, terrainData.size() * sizeof(GLfloat),terrainData.data(), GL_STATIC_DRAW);

            glGenVertexArrays(size, &terrainVAO);
            glBindVertexArray(terrainVAO);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                                  reinterpret_cast<void *>(6 * sizeof(GLfloat)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            info.terrainVAO = terrainVAO;
            info.terrainVBO = terrainVBO;
            info.row = row ;
            info.col = col ;
            info.terrainData = terrainData;
            info.trees = terrain.getTrees();
            info.pond = terrain.m_ponds;
            terrainRowMap[row][col] = info;
        }
    }
}
