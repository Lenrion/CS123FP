#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


glm::vec3 Cone::crossProduct(glm::vec3 A, glm::vec3 B, glm::vec3 C){
    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 normal = glm::cross(AB, AC);
    return glm::normalize(normal);
}

void Cone::setVertexData() {
    float angleStep = glm::radians(360.f / m_param2);
    glm::vec3 apex(0, m_height/2, 0); // Top point of the cone
    for (int i = 0; i < m_param2; ++i) {
        float angle = i * angleStep;
        float nextAngle = (i + 1) * angleStep;
        float radiusIncrement = m_radius / m_param1;
        float radius = 0;
        float heightIncrement = m_height / m_param1;
        float height = 0.5;
        for (int j = 0; j < m_param1; j++) {

            glm::vec3 v1(radius * sin(angle), height, radius * cos(angle)); //top left
            glm::vec3 v2(radius * sin(nextAngle), height, radius * cos(nextAngle)); //top right
            radius += radiusIncrement;
            height -= heightIncrement;

            glm::vec3 v3(radius * sin(angle), height, radius * cos(angle)); //bottom left
            glm::vec3 v4(radius * sin(nextAngle), height, radius * cos(nextAngle)); //bottom right

            if (j == 0) {
                makeTopTile(v1, v2, v3, v4, angle, nextAngle);
            } else {

                makeTile(v1, v2, v3, v4);
            }
        }
    }

    makeBase();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cone::makeTopTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, float currAngle, float nextAngle){
    float middleAngle = (currAngle + nextAngle)/2.f;
    glm::vec3 tipNormal = glm::vec3(sin(middleAngle), 0.f, cos(middleAngle));
    tipNormal = normalize(tipNormal);
    tipNormal.y = 0.5f;

    glm::vec3 norm1 = normalize(glm::vec3(v1.x, 0.f, v1.z));
    norm1 = glm::vec3(norm1.x, 0.5f,norm1.z);
    glm::vec3 norm2 = normalize(glm::vec3(v2.x, 0.f, v2.z));
    norm2 = glm::vec3(norm2.x, 0.5f,norm2.z);
    glm::vec3 norm3 = normalize(glm::vec3(v3.x, 0.f, v3.z));
    norm3 = glm::vec3(norm3.x, 0.5f,norm3.z);
    glm::vec3 norm4 = normalize(glm::vec3(v4.x, 0.f, v4.z));
    norm4 = glm::vec3(norm4.x, 0.5f,norm4.z);

    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, tipNormal);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, norm3);
    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, norm4);



    insertVec3(m_vertexData, v4);
    insertVec3(m_vertexData, norm4);
    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, tipNormal);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, norm3);


    // Second triangle


}


void Cone::makeTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
    // Normals for the slanted sides of the cone
    glm::vec3 norm1 = normalize(glm::vec3(v1.x, 0.f, v1.z));
    norm1 = glm::vec3(norm1.x, 0.5f,norm1.z);
    glm::vec3 norm2 = normalize(glm::vec3(v2.x, 0.f, v2.z));
    norm2 = glm::vec3(norm2.x, 0.5f,norm2.z);
    glm::vec3 norm3 = normalize(glm::vec3(v3.x, 0.f, v3.z));
    norm3 = glm::vec3(norm3.x, 0.5f,norm3.z);
    glm::vec3 norm4 = normalize(glm::vec3(v4.x, 0.f, v4.z));
    norm4 = glm::vec3(norm4.x, 0.5f,norm4.z);



    // First triangle


    // Second triangle
    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, norm1);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, norm3);
    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, norm2);


    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, norm2);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, norm3);
    insertVec3(m_vertexData, v4);
    insertVec3(m_vertexData, norm4);


}

void Cone::makeBase() {
    float angleStep = glm::radians(360.0f / m_param2);
    glm::vec3 center(0, 0, 0); // Center of the base
    glm::vec3 normal(0, -1, 0); // Normal of the base


    for (int i = 0; i < m_param2; ++i) {
        float angle = i * angleStep;
        float nextAngle = (i + 1) * angleStep;

        float radiusIncrement = m_radius / m_param1;
        float radius = 0;


        for (int j = 0; j < m_param1; j++){

            glm::vec3 v1(radius * sin(angle), -0.5f, radius * cos(angle));
            glm::vec3 v2(radius * sin(nextAngle), -0.5f, radius * cos(nextAngle));
            radius += radiusIncrement;
            glm::vec3 v3(radius * sin(angle), -0.5f, radius * cos(angle));
            glm::vec3 v4(radius * sin(nextAngle), -0.5f, radius * cos(nextAngle));
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

        }
    }
}



