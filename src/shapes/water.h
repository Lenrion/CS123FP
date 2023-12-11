#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>


class Water
{
public:
    Water(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }
    GLuint m_waterVBO;
    GLuint m_waterVAO;
    std::vector<float> m_vertexData;

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();


    float m_radius = 0.5;

    int m_param1;
    int m_param2;
};

