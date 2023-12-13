#pragma once

#include "glm/fwd.hpp"
#include <GL/glew.h>
#include "shapes/LSystem.h"
#include <vector>
class Tree
{
public:
    Tree(){};
    Tree(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale);
    void updateParams(int param1);
    std::vector<float> generateShape() { return m_vertexData; }
    GLuint m_treeVao;
    GLuint m_treeVbo;
    glm::mat4 m_modelMatrix;


private:
    void generateBranch(LSystemState currentState);
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void generateRecursively();
    void generateLSystem();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void generateSubtree(float currentHeight, glm::mat4 totalTranslate, float angle, glm::vec3 axis);
    std::vector<float> m_vertexData;
    int m_param1;
    LSystem m_lSystem;
};
