#pragma once

#include <vector>
#include <glm/glm.hpp>

class Skybox
{
public:
    void updateParams(int param1);
    std::vector<float> generateShape() { return m_vertexData; }
    glm::vec3 m_position;

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,glm::vec3 normal);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    glm::vec3 calculateNormal(glm::vec3 A, glm::vec3 B, glm::vec3 C);
    glm::vec3 crossProduct(glm::vec3 A, glm::vec3 B, glm::vec3 C);

    std::vector<float> m_vertexData;
    int m_param1;
};

