#include "terraingenerator.h"

#include <cmath>
#include <iostream>
#include <stack>
#include "glm/glm.hpp"
#include "shapes/Water.h"
#include <GL/glew.h>
#include <random>

// Constructor
TerrainGenerator::TerrainGenerator()
{

    m_wireshade = false;
    m_resolution = 50;
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);
    std::srand(1230);

    // Populate random vector lookup table
    for (int i = 0; i < m_lookupSize; i++)
    {
        m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }
}

// Destructor
TerrainGenerator::~TerrainGenerator()
{
    m_randVecLookup.clear();
}

// Helper for generateTerrain()
void addPointToVector(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.y);
    vector.push_back(point.z);
    vector.push_back(point.x);


}

// Generates the geometry of the output triangle mesh
std::vector<float> TerrainGenerator::generateTerrain(float xOffset, float yOffset) {
    std::vector<float> verts;
    m_trees.clear();
    m_ponds.waterData.clear();
    verts.reserve(m_resolution * m_resolution * 6);

    std::vector<std::vector<bool>> isInValley(m_resolution, std::vector<bool>(m_resolution, false));
    float scaleFactor =1.0f;  // scale terrain

    for(int x = 0; x < m_resolution; x++) {
        for(int y = 0; y < m_resolution; y++) {
            int x1 = x;
            int y1 = y;

            int x2 = x + 1;
            int y2 = y + 1;

            glm::vec3 p1 = getPosition(x1 * scaleFactor, y1 * scaleFactor, xOffset, yOffset);
            glm::vec3 p2 = getPosition(x2 * scaleFactor, y1 * scaleFactor, xOffset, yOffset);
            glm::vec3 p3 = getPosition(x2 * scaleFactor, y2 * scaleFactor, xOffset, yOffset);
            glm::vec3 p4 = getPosition(x1 * scaleFactor, y2 * scaleFactor, xOffset, yOffset);

            glm::vec3 n1 = getNormal(x1* scaleFactor,y1* scaleFactor,xOffset, yOffset);
            glm::vec3 n2 = getNormal(x2* scaleFactor,y1* scaleFactor, xOffset, yOffset);
            glm::vec3 n3 = getNormal(x2* scaleFactor,y2* scaleFactor, xOffset, yOffset);
            glm::vec3 n4 = getNormal(x1* scaleFactor,y2* scaleFactor, xOffset, yOffset);


            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p2, verts);
            addPointToVector(n2, verts);
            addPointToVector(getColor(n2, p2), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            addPointToVector(p4, verts);
            addPointToVector(n4, verts);
            addPointToVector(getColor(n4, p4), verts);

            if (p1.z < m_waterHeight || p2.z < m_waterHeight || p3.z < m_waterHeight || p4.z < m_waterHeight) {
                isInValley[x][y] = true;
            } else{
                generateTrees(p3);
            }

        }
    }
    createPond(isInValley, xOffset, yOffset);
    return verts;
}

void TerrainGenerator::createPond(const std::vector<std::vector<bool>>& isInValley, int xOffset, int yOffset) {
    // Find the minimum and maximum x and y coordinates
    int minX = m_resolution, minY = m_resolution, maxX = 0, maxY = 0;
    for (int x = 0; x < m_resolution; x++) {
        for (int y = 0; y < m_resolution; y++) {
            if (isInValley[x][y]) {
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }
        }
    }

    // Calculate the corners of the plane
    glm::vec3 v1 = glm::vec3((minY / (float)m_resolution) + yOffset, m_waterHeight, (minX / (float)m_resolution) + xOffset); // bottom left back
    glm::vec3 v2 = glm::vec3((maxY / (float)m_resolution) + yOffset, m_waterHeight, (minX / (float)m_resolution) + xOffset); // bottom right back
    glm::vec3 v3 = glm::vec3((minY / (float)m_resolution) + yOffset, m_waterHeight, (maxX / (float)m_resolution)+ xOffset); // top left front
    glm::vec3 v4 = glm::vec3((maxY / (float)m_resolution) + yOffset, m_waterHeight, (maxX / (float)m_resolution) + xOffset); // top right front

    // Create a Water object with these corners
    Water water(v1, v2, v3, v4);
    m_ponds.waterVAO = water.m_waterVAO;
    m_ponds.waterVBO = water.m_waterVBO;
    m_ponds.waterData = water.generateShape();

    // Store the Water object for later use

}



// Samples the (infinite) random vector grid at (row, col)
glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col)
{
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
// Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
glm::vec3 TerrainGenerator::getPosition(int row, int col, float xOffset, float yOffset) {
    // Normalizing the planar coordinates to a unit square
    // makes scaling independent of sampling resolution.
    float x = (1.0 * row / m_resolution) + xOffset;
    float y = (1.0 * col / m_resolution ) + yOffset;
    float z = getHeight(x, y);
    return glm::vec3(x,y,z);
}


float ease(float alpha){
    return 3.f* (alpha * alpha) - 2.f* alpha * alpha * alpha;
}


// Helper for computePerlin() and, possibly, getColor()
float interpolate(float A, float B, float alpha) {
    float output = A + (ease(alpha) * (B - A));
    return output;
}




// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float TerrainGenerator::getHeight(float x, float y) {
    if(x < 0){
        x *= -1;
    }
    if(y < 0){
        y *= -1;
    }

    // Task 6: modify this call to produce noise of a different frequency
    float z = computePerlin(x * 4, y * 4) / 4;

    // Combine multiple octaves of noise to produce fractal Perlin noise
    float a = computePerlin(x * 2, y * 2) / 2;
    float b = computePerlin(x * 1, y * 1) / 1;
    float c = computePerlin(x * 0.5f, y * 0.5f) / 0.5f;

    // Combine the octaves
    float d = a + b + c;

    return d;
}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 TerrainGenerator::getNormal(int row, int col, float xOffset, float yOffset) {
    // Task 9: Compute the average normal for the given input indices
    glm::vec3 normal = glm::vec3(0, 0, 0);
    std::vector<std::vector<int>> neighborOffsets = { // Counter-clockwise around the vertex
        {-1, -1},
        { 0, -1},
        { 1, -1},
        { 1,  0},
        { 1,  1},
        { 0,  1},
        {-1,  1},
        {-1,  0}
    };
    glm::vec3 V = getPosition(row,col,xOffset, yOffset);
    for (int i = 0; i < 8; ++i) {
        int n1RowOffset = neighborOffsets[i][0];
        int n1ColOffset = neighborOffsets[i][1];
        int n2RowOffset = neighborOffsets[(i + 1) % 8][0];
        int n2ColOffset = neighborOffsets[(i + 1) % 8][1];
        glm::vec3 n1 = getPosition(row + n1RowOffset, col + n1ColOffset, xOffset, yOffset);
        glm::vec3 n2 = getPosition(row + n2RowOffset, col + n2ColOffset, xOffset, yOffset);
        normal = normal + glm::cross(n1 - V, n2 - V);
    }
    return glm::normalize(normal);
}

// Computes color of vertex using normal and, optionally, position
glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    glm::vec3 green = glm::vec3(0.199,0.196,0.339);
    glm::vec3 darkGreen = glm::vec3(0.4,0.4,0.67);
    glm::vec3 sand = glm::vec3(0.53f,0.72f,0.55f);


    glm::vec3 upward_direction = glm::vec3(0, 1, 0);

    if(position.z > -0.3f && position.z < 0.f){
        return sand;
    }
    if(glm::dot(normal,upward_direction) > 0.005f && position.z > 0.01){
        return darkGreen;
    } else {
        return green;
    }

    // Return white as placeholder
    return glm::vec3(0.5f,0.5f,0.5f);
}

// Computes the intensity of Perlin noise at some point
float TerrainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    int a = (int) x; //x coordinate 1
    int b = (int) y; // y coordinate 1
    int c = a++;   //x coordinate 2
    int d = b++;  // y coordinate 2


    // Task 2: compute offset vectors
    glm::vec2 veca = glm::vec2(((float) a) - x,((float) b) - y ); //topleft
    glm::vec2 vecb = glm::vec2(((float) c) - x,((float) b) - y ); //topright
    glm::vec2 vecc = glm::vec2(((float) a) - x,((float) d) - y ); //bottomleft
    glm::vec2 vecd = glm::vec2(((float) c) - x,((float) d) - y ); //bottomright

    // Task 3: compute the dot product between the grid point direction vectors and its offset vectors
    float A = glm::dot(veca, sampleRandomVector(a,b));// dot product between top-left direction and its offset
    float B = glm::dot(vecb, sampleRandomVector(c,b)); // dot product between top-right direction and its offset
    float C = glm::dot(vecd, sampleRandomVector(c,d)); // dot product between bottom-right direction and its offset
    float D = glm::dot(vecc, sampleRandomVector(a,d)); // dot product between bottom-left direction and its offset

    // Task 5: Debug this line to properly use your interpolation function to produce the correct value
    return interpolate(interpolate(A, B, abs((float) a) - x), interpolate(D, C, abs((float) a) - x), abs((float) b) - y);
    //    return interpolate(interpolate(A, B, 0.5), interpolate(D, C, 0.5), 0.5);

    // Return 0 as a placeholder
}

void TerrainGenerator::generateTrees(glm::vec3 pos){
    std::mt19937 engine{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(1, 300);
    std::uniform_int_distribution<int> scaleX(30, 60);
    std::uniform_int_distribution<int> scaleY(5, 20);
    std::uniform_int_distribution<int> scaleZ(30, 60);
    int randInt = dist(engine);
    float randXScale = (float)scaleX(engine) /100.f;
    float randYScale = (float)scaleY(engine) /100.f;
    float randZScale = (float)scaleZ(engine) /100.f;

    if(randInt == 1){
        Tree tree = Tree(glm::vec3(pos.y, pos.z, pos.x), glm::vec3(0,1,0), glm::vec3(0.1f,randYScale,0.1f));
        TreeInfo treeInfo;
        treeInfo.treeVAO = tree.m_treeVao;
        treeInfo.treeVBO = tree.m_treeVbo;
        treeInfo.modelMat = tree.m_modelMatrix;
        treeInfo.treeData = tree.generateShape();
        m_trees.push_back(treeInfo);
    }


}

std::vector<TerrainGenerator::TreeInfo> TerrainGenerator::getTrees(){
    return m_trees;
}
