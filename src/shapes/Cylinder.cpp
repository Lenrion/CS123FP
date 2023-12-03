#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    if(param2 > 2){
        m_param2 = param2;
    }
    setVertexData();
}


void Cylinder::setVertexData() {
    float heightStep = m_height / m_param1;
    float angleStep = glm::radians(360.f / m_param2);


    makeRing(-0.5f, m_radius, false); // Bottom cap
    makeRing(0.5f, m_radius, true); // Top cap


    for (int i = 0; i < m_param2; ++i) {

        float currentAngle = i * angleStep;
        float nextAngle = (i + 1) * angleStep;

        for (int j = 0; j < m_param1; ++j) {
            float currentHeight = j * heightStep - 0.5f;
            float nextHeight = (j + 1) * heightStep -0.5f;

            glm::vec3 topLeft(m_radius * sin(currentAngle), currentHeight, m_radius * cos(currentAngle));
            glm::vec3 topRight(m_radius * sin(nextAngle), currentHeight, m_radius * cos(nextAngle));
            glm::vec3 bottomLeft(m_radius * sin(currentAngle), nextHeight, m_radius * cos(currentAngle));
            glm::vec3 bottomRight(m_radius * sin(nextAngle), nextHeight, m_radius * cos(nextAngle));

            makeTile(topLeft, topRight, bottomLeft, bottomRight);
        }
    }
}

void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}



void Cylinder::makeTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight) {
    glm::vec3 normalTopLeft = glm::normalize(glm::vec3(topLeft[0], 0.f, topLeft[2]));
    glm::vec3 normalTopRight = glm::normalize(glm::vec3(topRight[0], 0.f, topRight[2]));
    glm::vec3 normalBottomLeft = glm::normalize(glm::vec3(bottomLeft[0], 0.f, bottomLeft[2]));
    glm::vec3 normalBottomRight = glm::normalize(glm::vec3(bottomRight[0], 0.f, bottomRight[2]));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData,normalTopLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData,normalBottomRight);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData,normalBottomLeft);


    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData,normalBottomRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData,normalTopLeft);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData,normalTopRight);

}

void Cylinder::makeRing(float height, float radius, bool isTop) {
    glm::vec3 normal;
    float Yval;
    if(isTop){
        normal = glm::vec3(0, 1, 0);
        Yval = 0.5f;
    } else{
        normal = glm::vec3(0, -1, 0);
        Yval = -0.5f;
    }
    float angleStep = glm::radians(360.0f / m_param2);

    for (int i = 0; i < m_param2; ++i) {
        float angle = i * angleStep;
        float nextAngle = (i + 1) * angleStep;

        float radiusIncrement = m_radius / (float)m_param1;
        float radius = 0;


        for (int j = 0; j < m_param1; j++){

            glm::vec3 v1(radius * sin(angle), Yval, radius * cos(angle));
            glm::vec3 v2(radius * sin(nextAngle), Yval, radius * cos(nextAngle));
            radius += radiusIncrement;
            glm::vec3 v3(radius * sin(angle), Yval, radius * cos(angle));
            glm::vec3 v4(radius * sin(nextAngle), Yval, radius * cos(nextAngle));
            if(!isTop){
                insertVec3(m_vertexData, v1);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v4);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v3);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v4);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v1);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v2);
                insertVec3(m_vertexData, normal);
            } else {
                insertVec3(m_vertexData, v1);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v3);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v4);
                insertVec3(m_vertexData, normal);

                insertVec3(m_vertexData, v4);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v2);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, v1);
                insertVec3(m_vertexData, normal);

            }


        }
    }
}

