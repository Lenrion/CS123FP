#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }
    void setCtm(glm::mat4 newCtm);
    void setColor(glm::vec3 newColor){ m_color = newColor; };

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
    void makeTileCaps(float currentTheta, float nextTheta);
    void makeRing(float height, float radius, bool isTop);

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
    float m_height = 1;
    glm::mat4 ctm = glm::mat4(1.0f);
    glm::vec3 m_color = glm::vec3(1.0f);
};
