#include "skybox.h"
#include "glm/ext/scalar_constants.hpp"

void Skybox::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

glm::vec3 Skybox::crossProduct(glm::vec3 A, glm::vec3 B, glm::vec3 C){
    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 normal = glm::cross(AB, AC);
    return glm::normalize(normal);
}

void Skybox::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, glm::vec3 normal) {
    glm::vec3 normalA = crossProduct(topRight, topLeft,bottomRight);
    glm::vec3 normalB = crossProduct(topLeft, bottomLeft,bottomRight);
    insertVec3(m_vertexData, topLeft + m_position);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topRight + m_position);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight + m_position);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight + m_position);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft + m_position);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topLeft + m_position);
    insertVec3(m_vertexData, normal);
}

void Skybox::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 horizSquareLen = (topRight - topLeft) / static_cast<float>(m_param1);
    glm::vec3 vertiSquareLen = (bottomLeft - topLeft) / static_cast<float>(m_param1);
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0;j < m_param1; j++) {
            // four corners of the current tile
            glm::vec3 tileTopLeft = topLeft + horizSquareLen * static_cast<float>(j) + vertiSquareLen * static_cast<float>(i);
            glm::vec3 tileTopRight = tileTopLeft + horizSquareLen;
            glm::vec3 tileBottomLeft = tileTopLeft + vertiSquareLen;
            glm::vec3 tileBottomRight = tileBottomLeft + horizSquareLen;
            glm::vec3 norm = crossProduct(tileTopLeft, tileBottomLeft, tileBottomRight);
            // Create the tile
            makeTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight, norm);
        }
    }
}

void Skybox::setVertexData() {
    glm::vec3 v1 = glm::vec3(-40.f,  40.f, 40.f); //top left front
    glm::vec3 v2 = glm::vec3( 40.f,  40.f, 40.f); //top right front
    glm::vec3 v3 = glm::vec3(-40.f, -40.f, 40.f); //bottom left front
    glm::vec3 v4 = glm::vec3( 40.f, -40.f, 40.f); //bottom right front
    glm::vec3 v5 = glm::vec3(-40.f,  40.f, -40.f); //bottom right back
    glm::vec3 v6 = glm::vec3( 40.f,  40.f, -40.f); //bottom left back
    glm::vec3 v7 = glm::vec3(-40.f, -40.f, -40.f); //top right back
    glm::vec3 v8 = glm::vec3( 40.f, -40.f, -40.f); //top left back (facing)

    // Front face
    makeFace(v1, v2, v3, v4);
    makeFace(v6,v5,v8,v7);
    makeFace(v5,v1,v7,v3);
    makeFace(v2,v6,v4,v8);
    makeFace(v5,v6,v1,v2);
    makeFace(v3,v4,v7,v8);
}

void Skybox::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
