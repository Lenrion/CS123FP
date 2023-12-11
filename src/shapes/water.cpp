#include "water.h"

#include <GL/glew.h>

Water::Water(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight){
    m_vertexData = std::vector<float>();
    setVertexData(topLeft, topRight, bottomLeft, bottomRight);

}

//void Water::updateParams(int param1, int param2) {
//    m_vertexData = std::vector<float>();
//    m_param1 = glm::max(param1,2);
//    m_param2 = glm::max(param2,2);
//    setVertexData(topLeft, topRight, bottomLeft, bottomRight);
//}

void Water::makeTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight) {

    glm::vec3 normalTopLeft = glm::normalize(topLeft);
    glm::vec3 normalTopRight = glm::normalize(topRight);
    glm::vec3 normalBottomLeft = glm::normalize(bottomLeft);
    glm::vec3 normalBottomRight = glm::normalize(bottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData,normalTopLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData,normalBottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData,normalBottomRight);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData,normalBottomRight);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData,normalTopRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData,normalTopLeft);


}




void Water::setVertexData(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight) {

//    glm::vec3 v1 = glm::vec3(-1.7f,  0.5f, 1.7f); //top left front
//    glm::vec3 v2 = glm::vec3( 1.7f,  0.5f, 1.7f); //top right front
//    glm::vec3 v5 = glm::vec3(-1.7f,  0.5f, -1.7f); //bottom right back
//    glm::vec3 v6 = glm::vec3( 1.7f,  0.5f, -1.7f); //bottom left back

    //    glm::vec3 v1 = glm::vec3(-1.7f,  0.5f, 1.7f); //top left front
    //    glm::vec3 v2 = glm::vec3( 1.7f,  0.5f, 1.7f); //top right front

    //    // Top face
//    makeTile(v5,v6,v1,v2);
    makeTile(topLeft, topRight, bottomLeft, bottomRight);


    // Generate and bind the VBO
    glGenBuffers(1, &m_waterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexData.size(), m_vertexData.data(), GL_STATIC_DRAW);

    // Generate and bind the VAO
    glGenVertexArrays(1, &m_waterVAO);
    glBindVertexArray(m_waterVAO);

    // Set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Water::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
