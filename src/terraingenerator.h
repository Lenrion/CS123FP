#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "shapes/Water.h"
#include "shapes/Tree.h"
#include <GL/glew.h>


class TerrainGenerator
{
public:
    bool m_wireshade;
    float m_waterHeight = -0.2f;


    TerrainGenerator();
    ~TerrainGenerator();
    int getResolution() { return m_resolution; };
    std::vector<float> generateTerrain(float xOffset, float yOffset);
    void createPond(const std::vector<std::vector<bool>>& isInValley,int xOffset, int yOffset);
    std::vector<glm::vec3> identifyPerimeter(int x, int y);
    struct TreeInfo{
        GLuint treeVAO;
        GLuint treeVBO;
        std::vector<float> treeData;
        float u;
        float v;
        glm::mat4 modelMat;
    };
    struct WaterInfo{
        GLuint waterVAO;
        GLuint waterVBO;
        std::vector<float> waterData;
    };

    WaterInfo m_ponds;

    std::vector<TreeInfo> getTrees();

private:

    // Member variables for terrain generation. You will not need to use these directly.
    std::vector<glm::vec2> m_randVecLookup;
    int m_resolution;
    int m_lookupSize;

    std::vector<TreeInfo> m_trees;


    // Samples the (infinite) random vector grid at (row, col)
    glm::vec2 sampleRandomVector(int row, int col);

    // Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
    // Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
    glm::vec3 getPosition(int row, int col,float xOffset, float yOffset);

    // ================== Students, please focus on the code below this point

    // Takes a normalized (x, y) position, in range [0,1)
    // Returns a height value, z, by sampling a noise function
    float getHeight(float x, float y);

    // Computes the normal of a vertex by averaging neighbors
    glm::vec3 getNormal(int row, int col, float xOffset, float yOffset);

    // Computes color of vertex using normal and, optionally, position
    glm::vec3 getColor(glm::vec3 normal, glm::vec3 position);


    // Computes the intensity of Perlin noise at some point
    float computePerlin(float x, float y);
    void generateTrees(glm::vec3 pos);


};

