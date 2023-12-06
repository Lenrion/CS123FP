#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include "debugger.h"
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "./utils/sceneparser.h"
#include "shapes/Sphere.h"
#include "shapes/Cone.h"
#include "shapes/Cylinder.h"
#include "shapes/Cube.h"
#include "camera.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void makeFBO();

    GLuint sphereVBO;
    GLuint sphereVAO;
    GLuint cylinderVBO;
    GLuint cylinderVAO;
    GLuint coneVBO;
    GLuint coneVAO;
    GLuint cubeVBO;
    GLuint cubeVAO;

    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;
    GLuint m_filterShader;

    std::vector<float> sphereData;
    std::vector<float> coneData;
    std::vector<float> cylinderData;
    std::vector<float> cubeData;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;


    GLuint shader = 0;
    Cone cone;
    Cube cube;
    Cylinder cylinder;
    Sphere sphere;
    float m_ka;
    float m_ks;
    float m_kd;

    void paintTexture(GLuint texture);

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;
    void createSphere();    // Stores whether keys are pressed or not
    void createCone();
    void createCube();
    void createCylinder();
    // Device Correction Variables
    int m_devicePixelRatio;
    RenderData renderData;
    Camera m_camera;
    int timerId = -1;
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);
};
