#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "shapes/skybox.h"
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
#include "shapes/Tree.h"
#include "camera.h"
#include "terraingenerator.h"

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
    GLuint loadCubemap(std::vector<std::string> faces);

    GLuint sphereVBO;
    GLuint sphereVAO;
    GLuint cylinderVBO;
    GLuint cylinderVAO;
    GLuint coneVBO;
    GLuint coneVAO;
    GLuint cubeVBO;
    GLuint cubeVAO;
    GLuint skyboxVBO;
    GLuint skyboxVAO;


    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_skybox_texture;

    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;


    std::vector<float> sphereData;
    std::vector<float> coneData;
    std::vector<float> cylinderData;
    std::vector<float> cubeData;
    std::vector<float> skyboxData;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    int chunks = 5;


    struct TerrainPatch{
        GLuint terrainVBO;
        GLuint terrainVAO;
        int row;
        int col;
        std::vector<float> terrainData;
        std::vector<TerrainGenerator::TreeInfo> trees;
        TerrainGenerator::WaterInfo pond;
    };


    GLuint shader = 0;
    GLuint m_filterShader;
    GLuint m_terrainShader;
    GLuint m_water_shader;
    GLuint m_skybox_shader;

    Cone cone;
    Cube cube;
    Cylinder cylinder;
    Skybox skybox;
    Sphere sphere;
    TerrainGenerator terrain;
    Tree m_tree;
    float m_ka = 0.5f;
    float m_ks = 0.5f;
    float m_kd = 0.5f;

    std::unordered_map<int, TerrainPatch> terrainMap;
    std::unordered_map<int, std::unordered_map<int, TerrainPatch>> terrainRowMap;


    void paintTexture(GLuint texture);
    void paintTrees(std::vector<TerrainGenerator::TreeInfo> trees);
    void paintWater(TerrainGenerator::WaterInfo water);

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
    void createTerrain(float xOffset, float yOffset);
    void createSkybox();

    // Device Correction Variables
    int m_devicePixelRatio;
    RenderData renderData;
    Camera m_camera;
    int timerId = -1;
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    bool m_isInitialized = false;



};
