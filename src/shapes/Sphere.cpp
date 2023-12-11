#include "Sphere.h"
#include "glm/ext/scalar_constants.hpp"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    if(param1 > 1){
        m_param1 = param1;
    }
    if(param2 > 1){
        m_param2 = param2;
    }

    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    glm::vec3 normalTopLeft = glm::normalize(topLeft);
    glm::vec3 normalTopRight = glm::normalize(topRight);
    glm::vec3 normalBottomLeft = glm::normalize(bottomLeft);
    glm::vec3 normalBottomRight = glm::normalize(bottomRight);

//    insertVec3(m_vertexData, topLeft);
//    insertVec3(m_vertexData,normalTopLeft);
//    insertVec3(m_vertexData, bottomLeft);
//    insertVec3(m_vertexData,normalBottomLeft);
//    insertVec3(m_vertexData, bottomRight);
//    insertVec3(m_vertexData,normalBottomRight);

//    insertVec3(m_vertexData, bottomRight);
//    insertVec3(m_vertexData,normalBottomRight);
//    insertVec3(m_vertexData, topRight);
//    insertVec3(m_vertexData,normalTopRight);
//    insertVec3(m_vertexData, topLeft);
//    insertVec3(m_vertexData,normalTopLeft);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    int numTiles = m_param1;
    float phiIncrement = glm::pi<float>() / static_cast<float>(numTiles);

    for (int i = 0; i < numTiles; ++i) {
        float currentPhi = static_cast<float>(i) * phiIncrement;
        float nextPhi = static_cast<float>(i + 1) * phiIncrement;

        float topLeftX = m_radius * sin(currentPhi)*sin(currentTheta);
        float topLeftY = m_radius * cos(currentPhi);
        float topLeftZ = m_radius * sin(currentPhi)*cos(currentTheta);
        glm::vec3 topLeft(topLeftX, topLeftY, topLeftZ);

        float topRightX = m_radius * sin(currentPhi) * sin(nextTheta);
        float topRightY = m_radius * cos(currentPhi);
        float topRightZ = m_radius * sin(currentPhi) * cos(nextTheta);
        glm::vec3 topRight(topRightX, topRightY, topRightZ);

        float bottomLeftX = m_radius * sin(nextPhi) * sin(currentTheta);
        float bottomLeftY = m_radius * cos(nextPhi);
        float bottomLeftZ = m_radius * sin(nextPhi) * cos(currentTheta);
        glm::vec3 bottomLeft(bottomLeftX, bottomLeftY, bottomLeftZ);

        float bottomRightX = m_radius * sin(nextPhi) * sin(nextTheta);
        float bottomRightY = m_radius * cos(nextPhi);
        float bottomRightZ = m_radius * sin(nextPhi) * cos(nextTheta);
        glm::vec3 bottomRight(bottomRightX, bottomRightY, bottomRightZ);

        makeTile(topLeft,topRight,bottomLeft,bottomRight);
    }
}

void Sphere::setVertexData() {
    int numWedges = m_param2;
    float thetaStep = glm::radians(360.f/numWedges);

    for (int i = 0; i < numWedges; ++i) {
        makeWedge(i*thetaStep,(i+1)*thetaStep);
    }
}

void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}


