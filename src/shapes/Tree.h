#pragma once

#include "glm/fwd.hpp"
#include "shapes/lsystem.h"
#include <vector>
#include <GL/glew.h>

class Tree
{
public:
    void updateParams(int param1);
    std::vector<float> generateShape() { return m_vertexData; }
    Tree(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale);
    Tree(){};
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
    void generateBushes();
    std::vector<float> m_vertexData;
    std::vector<LSystemState> m_terminalStates;
    int m_param1;
    LSystem m_lSystem;
};
