#include "terraingenerator.h"

#include <cmath>
#include <iostream>
#include <stack>
#include "glm/glm.hpp"
#include "shapes/Water.h"
#include <GL/glew.h>

// Constructor
TerrainGenerator::TerrainGenerator()
{
    // Task 8: turn off wireframe shading
    m_wireshade = false; // STENCIL CODE
    // m_wireshade = false; // TA SOLUTION

    // Define resolution of terrain generation
    m_resolution = 100;

    // Generate random vector lookup table
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);

    // Initialize random number generator
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
    verts.reserve(m_resolution * m_resolution * 6);

    std::vector<std::vector<bool>> isInValley(m_resolution, std::vector<bool>(m_resolution, false));
    float scaleFactor =3.0f;  // scale terrain

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

            if (p1.y < 0.1 || p2.y < 0.1 || p3.y < 0.1 || p4.y < 0.1) {
                isInValley[x][y] = true;
            }

        }
    }
    createPond(isInValley);
    return verts;
}

void TerrainGenerator::createPond(const std::vector<std::vector<bool>>& isInValley) {
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
    glm::vec3 v1 = glm::vec3(minX / (float)m_resolution, 0.1f, minY / (float)m_resolution); // bottom left back
    glm::vec3 v2 = glm::vec3(maxX / (float)m_resolution, 0.1f, minY / (float)m_resolution); // bottom right back
    glm::vec3 v3 = glm::vec3(minX / (float)m_resolution, 0.1f, maxY / (float)m_resolution); // top left front
    glm::vec3 v4 = glm::vec3(maxX / (float)m_resolution, 0.1f, maxY / (float)m_resolution); // top right front

    // Create a Water object with these corners
    Water water(v1, v2, v3, v4);

    // Store the Water object for later use
    m_ponds.push_back(water);
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

// ================== Students, please focus on the code below this point

float ease(float alpha){
    return 3.f* (alpha * alpha) - 2.f* alpha * alpha * alpha;
}


// Helper for computePerlin() and, possibly, getColor()

float interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    float output = A + (ease(alpha) * (B - A));

    // Return 0 as placeholder
    return output;
}




// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float TerrainGenerator::getHeight(float x, float y) {


    // Task 6: modify this call to produce noise of a different frequency
    float z = computePerlin(x * 4, y * 4) / 4;

    // Combine multiple octaves of noise to produce fractal Perlin noise
    float a = computePerlin(x * 2, y * 2) / 2;
    float b = computePerlin(x * 1, y * 1) / 1;
    float c = computePerlin(x * 0.5, y * 0.5) / 0.5;

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
    // Task 10: compute color as a function of the normal and position
//    uint8_t r = normal[0] * 0;
//    uint8_t g = normal[1] * 255;
//    uint8_t b = normal[2] * 0;

    //ORDER IS BLUE RED GREEN (wtf anastasio)
    glm::vec3 green = glm::vec3(0.199,0.196,0.339);
    glm::vec3 brown = glm::vec3(0.18,0.49,0.306);
    glm::vec3 upward_direction = glm::vec3(0, 1, 0);

    if(glm::dot(normal,upward_direction) > 0.005f && position.z > 0.01){
        return green;
    } else {
        return brown;

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
