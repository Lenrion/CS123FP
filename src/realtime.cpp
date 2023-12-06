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
#include "utils/shaderloader.h"
#include "camera.h"

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

    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    glDeleteProgram(shader);
    glDeleteProgram(m_filterShader);
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

    // Students: anything requiring OpenGL calls when the program starts should be done here


    GLsizei size = 1;
    cone = Cone();
    cylinder = Cylinder();
    sphere = Sphere();
    cube = Cube();
    createSphere();
    createCone();
    createCylinder();
    createCube();

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

}

void Realtime::paintGL() {

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjectionMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader);
    // Clear screen color and depth before painting
    GLint cameraWorldPosLocation  = glGetUniformLocation(shader, "cameraWorldPos");
    glUniform4fv(cameraWorldPosLocation,1.f,&(glm::inverse(m_view)*glm::vec4(0.f,0.f,0.f,1.f))[0]);

    GLint viewMatrixLocation = glGetUniformLocation(shader, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &m_view[0][0]);

    GLint projectionMatrixLocation = glGetUniformLocation(shader, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &m_proj[0][0]);

    GLint ksLocation = glGetUniformLocation(shader, "k_s");
    glUniform1f(ksLocation, m_ks);

    // Task 12: pass m_ka into the fragment shader as a uniform
    GLint kaLocation = glGetUniformLocation(shader, "k_a");
    glUniform1f(kaLocation, m_ka);

    GLint kdLocation = glGetUniformLocation(shader, "k_d");
    glUniform1f(kdLocation, m_kd);


    for(RenderShapeData shape: renderData.shapes){

        GLint ambientLocation = glGetUniformLocation(shader, "ambient");
        glUniform4fv(ambientLocation,1.f, &shape.primitive.material.cAmbient[0]);

        GLint diffuseLocation = glGetUniformLocation(shader, "diffuse");
        glUniform4fv(diffuseLocation,1.f , &shape.primitive.material.cDiffuse[0]);

        GLint specularLocation = glGetUniformLocation(shader, "specular");
        glUniform4fv(specularLocation, 1.f, &shape.primitive.material.cSpecular[0]);

        GLint shininessLocation = glGetUniformLocation(shader, "shininess");
        glUniform1f(shininessLocation, shape.primitive.material.shininess);


        GLint modelMatrixLocation;
        GLint inverseTransposeMatrixLocation;
        switch(shape.primitive.type){
            case PrimitiveType::PRIMITIVE_SPHERE:
                glBindVertexArray(sphereVAO);
                m_model = shape.ctm;
                modelMatrixLocation = glGetUniformLocation(shader, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_model[0][0]);
                inverseTransposeMatrixLocation = glGetUniformLocation(shader, "inverseTransposeMatrix");
                glUniformMatrix4fv(inverseTransposeMatrixLocation, 1, GL_FALSE, &glm::inverse(glm::transpose(m_model))[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, sphereData.size() / 6);

                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                glBindVertexArray(cylinderVAO);
                m_model = shape.ctm;
                modelMatrixLocation = glGetUniformLocation(shader, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_model[0][0]);
                inverseTransposeMatrixLocation = glGetUniformLocation(shader, "inverseTransposeMatrix");
                glUniformMatrix4fv(inverseTransposeMatrixLocation, 1, GL_FALSE, &glm::inverse(glm::transpose(m_model))[0][0]);

                glDrawArrays(GL_TRIANGLES, 0, cylinderData.size() / 6);
                break;
            case PrimitiveType::PRIMITIVE_CUBE:
                glBindVertexArray(cubeVAO);
                m_model = shape.ctm;
                modelMatrixLocation = glGetUniformLocation(shader, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_model[0][0]);
                inverseTransposeMatrixLocation = glGetUniformLocation(shader, "inverseTransposeMatrix");
                glUniformMatrix4fv(inverseTransposeMatrixLocation, 1, GL_FALSE, &glm::inverse(glm::transpose(m_model))[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, cubeData.size() / 3);
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                glBindVertexArray(coneVAO);
                m_model = shape.ctm;
                modelMatrixLocation = glGetUniformLocation(shader, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_model[0][0]);
                inverseTransposeMatrixLocation = glGetUniformLocation(shader, "inverseTransposeMatrix");
                glUniformMatrix4fv(inverseTransposeMatrixLocation, 1, GL_FALSE, &glm::inverse(glm::transpose(m_model))[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, coneData.size() / 3);
                break;
            }


        glBindVertexArray(0);


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
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_fbo_texture);
    glUniform1i(glGetUniformLocation(m_filterShader,"tex"), 0);


    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
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
        std::vector<SceneLightData> lights = renderData.lights;
        int numLights = 0;
       glUseProgram(shader);
       for(int i = 0; i < lights.size(); i++){
            //pos
                int type;
                switch(lights[i].type) {
                case LightType::LIGHT_DIRECTIONAL:
                    type = 0;
                    break;
                case LightType::LIGHT_POINT:
                    type = 1;
                    break;
                case LightType::LIGHT_SPOT:
                    type = 2;
                    std::string lightAngleName = "lights[" + std::to_string(i) + "].angle";
                    GLint lightAngleLoc = glGetUniformLocation(shader, lightAngleName.c_str());
                    glUniform1f(lightAngleLoc,lights[i].angle);
                    std::string lightPenumbraName = "lights[" + std::to_string(i) + "].penumbra";
                    GLint lightPenumbraLoc = glGetUniformLocation(shader, lightPenumbraName.c_str());
                    glUniform1f(lightPenumbraLoc,lights[i].penumbra);
                    break;
                }

                std::string lightAName = "lights[" + std::to_string(i) + "].a";
                GLint lightALoc = glGetUniformLocation(shader, lightAName.c_str());
                glUniform1f(lightALoc,lights[i].function[0]);

                std::string lightBName = "lights[" + std::to_string(i) + "].b";
                GLint lightBLoc = glGetUniformLocation(shader, lightBName.c_str());
                glUniform1f(lightBLoc,lights[i].function[1]);

                std::string lightCName = "lights[" + std::to_string(i) + "].c";
                GLint lightCLoc = glGetUniformLocation(shader, lightCName.c_str());
                glUniform1f(lightCLoc,lights[i].function[2]);


                std::string lightTypeName = "lights[" + std::to_string(i) + "].type";
                GLint lightTypeLoc = glGetUniformLocation(shader, lightTypeName.c_str());
                glUniform1i(lightTypeLoc,type);

                std::string lightPosName = "lights[" + std::to_string(i) + "].pos";
                GLint lightPosLoc = glGetUniformLocation(shader, lightPosName.c_str());
                glUniform4fv(lightPosLoc,1.f, &renderData.lights[i].pos[0]);
                //dir
                std::string lightDirName = "lights[" + std::to_string(i) + "].dir";
                GLint lightDirLoc = glGetUniformLocation(shader, lightDirName.c_str());
                glUniform4fv(lightDirLoc,1.f, &renderData.lights[i].dir[0]);
                //color
                std::string lightColorName = "lights[" + std::to_string(i) + "].color";
                GLint lightColorLoc = glGetUniformLocation(shader, lightColorName.c_str());
                glUniform4fv(lightColorLoc, 1.f, &renderData.lights[i].color[0]);
                numLights++;


       }
       glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);
       glUseProgram(0);

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
        float x = deltaX / 25.f;
        float y = deltaY / 25.f;
        m_camera.rotateCamera(x, y);
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