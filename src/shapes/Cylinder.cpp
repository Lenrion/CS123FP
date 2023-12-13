#include "Cylinder.h"
#include "glm/gtx/transform.hpp"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    //    glm::mat4 rotate = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0, 0));
    //    glm::mat4 scale = glm::scale(glm::vec3(0.2, 2.0, 0.2));
    //    glm::mat4 translate = glm::translate(glm::vec3(0, 0.5f, 0.0));
    //    setCtm(translate*rotate*scale);
    setVertexData();
}

void Cylinder::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight){

    insertVec3(m_vertexData, ctm*glm::vec4(topLeft, 1.0f)); //pos
    insertVec3(m_vertexData, glm::vec3(2.0f*topLeft.x, 0, 2.0f*topLeft.z)); //normal
    insertVec3(m_vertexData, m_color); //color

    insertVec3(m_vertexData, ctm*glm::vec4(bottomRight, 1.0f));
    insertVec3(m_vertexData, glm::vec3(2.0f*bottomRight.x, 0, 2.0f*bottomRight.z));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(bottomLeft, 1.0f));
    insertVec3(m_vertexData, glm::vec3(2.0f*bottomLeft.x, 0, 2.0f*bottomLeft.z));
    insertVec3(m_vertexData, m_color); //color



    //triangle two
    insertVec3(m_vertexData, ctm*glm::vec4(topRight, 1.0f));
    insertVec3(m_vertexData, glm::vec3(2.0f*topRight.x, 0, 2.0f*topRight.z));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(bottomRight, 1.0f));
    insertVec3(m_vertexData, glm::vec3(2.0f*bottomRight.x, 0, 2.0f*bottomRight.z));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(topLeft, 1.0f));
    insertVec3(m_vertexData, glm::vec3(2.0f*topLeft.x, 0, 2.0f*topLeft.z));
    insertVec3(m_vertexData, m_color); //color

}

void Cylinder::setCtm(glm::mat4 newCtm){
    ctm = newCtm;
}
void Cylinder::makeTileCaps(float currentTheta, float nextTheta){
    float x1 = 0.5f * cos(currentTheta);
    float z1 = 0.5f * sin(currentTheta);

    float x2 = 0.5f * cos(nextTheta);
    float z2 = 0.5f * sin(nextTheta);

    insertVec3(m_vertexData, ctm*glm::vec4(x1, 0.5, z1, 1.0f));
    insertVec3(m_vertexData, glm::vec3(0.0, 0.5, 0.0));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(0.0, 0.5, 0.0, 1.0f));
    insertVec3(m_vertexData, glm::vec3(0.0, 0.5, 0.0));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(x2, 0.5, z2, 1.0f));
    insertVec3(m_vertexData, glm::vec3(0.0, 0.5, 0.0));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(x1, -0.5, z1, 1.0f));
    insertVec3(m_vertexData, glm::vec3(0.0, -0.5, 0.0));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(x2, -0.5, z2, 1.0f));
    insertVec3(m_vertexData, glm::vec3(0.0, -0.5, 0.0));
    insertVec3(m_vertexData, m_color); //color


    insertVec3(m_vertexData, ctm*glm::vec4(0.0, -0.5, 0.0, 1.0f));
    insertVec3(m_vertexData, glm::vec3(0.0, -0.5, 0.0));
    insertVec3(m_vertexData, m_color); //color


}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    //param two controls the number of faces that radially make up the cylinder
    //param one controls the number of slices made for the rectangle faces
    int numFaces = fmax(3, m_param2);

    float thetaStep = glm::radians(360.0f / float(numFaces));
    float sliceHeight = 1.0f / float(m_param1);

    for(int i = 0; i < numFaces; i++){
        float currentTheta = thetaStep*i;
        float nextTheta = currentTheta + thetaStep;
        makeTileCaps(currentTheta, nextTheta);
        //update to match TA output (same for cone)
        for(int j = 0; j < m_param1; j++){
            float currentHeight = j*sliceHeight - 0.5f;
            float topHeight = currentHeight + sliceHeight;
            glm::vec3 topLeftPos = glm::vec3(0.5f * cos(currentTheta), topHeight, 0.5f*sin(currentTheta));
            glm::vec3 topRightPos = glm::vec3(0.5f * cos(nextTheta), topHeight, 0.5f*sin(nextTheta));
            glm::vec3 bottomLeftPos = glm::vec3(0.5f * cos(currentTheta), currentHeight, 0.5f * sin(currentTheta));
            glm::vec3 bottomRightPos = glm::vec3(0.5f* cos(nextTheta), currentHeight, 0.5f*sin(nextTheta));
            makeTile(topLeftPos, topRightPos, bottomLeftPos, bottomRightPos);
        }
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
