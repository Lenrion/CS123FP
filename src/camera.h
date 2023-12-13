#pragma once

#include <glm/glm.hpp>
#include "utils/scenedata.h"
#include "utils/sceneparser.h"

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    Camera();
    Camera(SceneCameraData cameraData, int width, int height);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getInverseViewMatrix() const;
    void setViewMatrix(glm::mat4 newViewMatrix);

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;
    glm::vec3 getCameraPos();

    void calculateViewMatrix();
    void calculateProjectionMatrix();
    void moveLook(bool isBack);
    void update(SceneCameraData cameraData, int width, int height);
    void updateCameraPos(glm::vec3 offSet);
    void rotateCamera(float deltaX, float deltaY);

    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;
    glm::mat4 projectionMatrix;
    float heightAngle;
    float aspectRatio;
    glm::vec3 pos;
    glm::vec3 look;
    glm::vec3 up;
    SceneCameraData m_cameraData;
    int m_width;
    int m_height;
    glm::mat4 constructRotationMatrix(const glm::vec3& axis, float theta);




};
