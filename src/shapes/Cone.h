#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
    void makeTopTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, float currAngle, float nextAngle);
    void makeBase();
    glm::vec3 crossProduct(glm::vec3 A, glm::vec3 B, glm::vec3 C);


    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
    float m_height = 1;

};
