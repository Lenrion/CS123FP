#include <stdexcept>
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "settings.h"
#include <iostream>
#include "utils/sceneparser.h"

Camera::Camera(){}

Camera::Camera(SceneCameraData cameraData, int width, int height) {
    look = cameraData.look;
    up = cameraData.up;
    pos = cameraData.pos;
    m_cameraData = cameraData;
    m_height = height;
    m_width = width;
    calculateViewMatrix();
    calculateProjectionMatrix();
}

void Camera::update(SceneCameraData cameraData, int width, int height){
    calculateViewMatrix();
    calculateProjectionMatrix();
}

glm::vec3 Camera::getCameraPos(){
    return pos;
}




glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return inverseViewMatrix;
}


glm::mat4 Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

float Camera::getAspectRatio() const {
    return aspectRatio;
}

float Camera::getHeightAngle() const {
    return heightAngle;
}

float Camera::getFocalLength() const {
    throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
    throw std::runtime_error("not implemented");
}

void Camera::calculateViewMatrix(){
    glm::vec3 w = -1.f * glm::normalize(look);
    glm::vec3 v = glm::normalize(up - glm::dot(up, w) * w);
    glm::vec3 u = glm::cross(v,w);
    glm::vec3 p = pos;
    glm::mat4 R = glm::mat4(u.x, v.x , w.x , 0.f, u.y,v.y, w.y, 0.f, u.z ,v.z,w.z, 0.f, 0.f, 0.f, 0.f, 1.0f);
    glm::mat4 T = glm::mat4(1.0f, 0 , 0 , 0, 0, 1.0f, 0, 0, 0 ,0,1.0f, 0, -p.x, -p.y, -p.z, 1.0f);
    glm::mat4 V = R * T;
    viewMatrix = V;
    inverseViewMatrix = glm::inverse(V);
}

void Camera::updateCameraPos(glm::vec3 offset){
    pos += offset;
    calculateViewMatrix();
    calculateProjectionMatrix();
}

void Camera::rotateCamera(float deltaX, float deltaY){

    glm::vec3 u = glm::cross(look, up);
    glm::mat3 xRotationMatrix = glm::mat3( cos(deltaX), 0.f                             , -sin(deltaX),
                                           0.f        , cos(deltaX) + (1 - cos(deltaX)) , 0.f,
                                          sin(deltaX) , 0.f                             ,  cos(deltaX) );



    glm::mat3 yRotationMatrix = glm::mat3(  cos(deltaY) + u.x *u.x * (1 - cos(deltaY))    , u.x*u.y*(1-cos(deltaY))+u.z*sin(deltaY)           , u.x*u.z*(1-cos(deltaY))-u.y*sin(deltaY),
                                            u.x*u.y*(1-cos(deltaY))-u.z*sin(deltaY)       , cos(deltaY) + u.y*u.y*(1 - cos(deltaY))           , u.y*u.z*(1-cos(deltaY) + u.x*sin(deltaY)),
                                            u.x*u.z*(1-cos(deltaY))+ u.y* sin(deltaY)     , u.y * u.z* (1 - cos(deltaY)) -u.x * sin(deltaY)   , cos(deltaY) + u.z *u.z*(1-cos(deltaY)));
    look = glm::normalize(look * xRotationMatrix *yRotationMatrix);
    up = glm::normalize(up *  xRotationMatrix * yRotationMatrix);

    calculateViewMatrix();
    calculateProjectionMatrix();
}


void Camera::calculateProjectionMatrix(){
    heightAngle = m_cameraData.heightAngle;
    float floatywidth = float(m_width);
    float floatyheight = float(m_height);
    aspectRatio = floatywidth/floatyheight;
    float c = -settings.nearPlane/settings.farPlane;
    float viewPlaneHeight = tan(heightAngle/2.f);
    float viewPlaneWidth = viewPlaneHeight * aspectRatio;

    glm::mat4 project1 = glm::mat4(1.f,0.f,0.f,0.f,
                                   0.f,1.f,0.f,0.f,
                                   0.f,0.f,-2.f,0.f,
                                   0.f,0.f,-1.f,1.f);

    glm::mat4 project2 = glm::mat4(1.f,0.f,0.f,0.f,
                                   0.f,1.f,0.f,0.f,
                                   0.f,0.f,1.f/(1.f+c),-1.f,
                                   0.f,0.f,-c/(1.f+c),0.f);
    glm::mat4 project3 = glm::mat4(1.f/(settings.farPlane * viewPlaneWidth),0.f,0.f,0.f,
                                   0.f,1.f/(settings.farPlane * viewPlaneHeight),0.f,0.f,
                                   0.f,0.f,1.f/settings.farPlane,0.f,
                                   0.f,0.f,0.f,1.f);


    projectionMatrix =  project1 * project2 * project3;
}
