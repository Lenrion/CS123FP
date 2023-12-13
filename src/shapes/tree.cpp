#include "Tree.h"
#include "glm/gtx/transform.hpp"
#include "shapes/Cylinder.h"
#include <ctime>

Tree::Tree(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale){
    glm::mat4 translate = glm::translate(position);
    glm::vec3 yAxis = glm::vec3(0, 1.0f, 0);
    glm::vec3 treeAxis = glm::normalize(orientation);
    float thetaRotation = acos(glm::dot(treeAxis, yAxis) /
                               (glm::length(treeAxis) * glm::length(yAxis)));
    glm::vec3 axisOfRotation = glm::cross(yAxis, treeAxis);
    glm::mat4 rotate = glm::mat4(1.0f);
    if(glm::length(axisOfRotation) != 0.0){
        rotate = glm::rotate(thetaRotation, axisOfRotation);
    }
    glm::mat4 scaleMat = glm::scale(scale);
    m_modelMatrix = translate*rotate*scaleMat;

    //populate vertex data
    updateParams(3);
}

void Tree::updateParams(int param1){
    if(param1 != m_param1){
        m_vertexData = std::vector<float>();
        m_param1 = std::min(5, param1);
        m_lSystem = LSystem();
        m_lSystem.applyRules(m_param1);
        setVertexData();
    }
}


void Tree::setVertexData(){

    srand((unsigned) time(NULL));

    //    generateRecursively();
    generateLSystem();
    generateBushes();
    //next, we want to recursive generate branches
    //at each level of a branch, the next branch should take, say, 80% of the height.
    //the starting y value of the next branch can be the maximum y value.
    glGenBuffers(1, &m_treeVbo);

    glBindBuffer(GL_ARRAY_BUFFER, m_treeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexData.size(), m_vertexData.data(), GL_STATIC_DRAW);

    // Generate and bind the VAO
    glGenVertexArrays(1, &m_treeVao);
    glBindVertexArray(m_treeVao);

    // Set the vertex attributes pointers
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //    //color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Tree::generateBushes(){
    std::vector<LSystemState> terminatedStates = m_lSystem.getTerminatedStates();
    for(LSystemState state : terminatedStates){
        Cylinder bush = Cylinder();
        glm::mat4 scale = glm::scale(glm::vec3(1.0, 0.2, 1.0));
        bush.setCtm(state.translateMatrix * scale);
        bush.setColor(glm::vec3(0.1f, 0.8f, 0.2f));
        bush.updateParams(5, 5);
        std::vector<float> newData = bush.generateShape();
        m_vertexData.insert(m_vertexData.end(), newData.begin(), newData.end());
    }
}

void Tree::generateLSystem(){
    std::string sentence = m_lSystem.getSentence();
    int count = 0;

    for(char c : sentence){
        switch(c){
        case 'F': { //draw a cylinder using the current state and update the state.
            generateBranch(m_lSystem.getCurrentState());
            m_lSystem.generatedBranch(true);
        }
        break;
        case '+':
            m_lSystem.rotateFacing(glm::radians(25.0f));
            break;
        case '-':
            m_lSystem.rotateFacing(glm::radians(-25.0f));
            break;
        case '[':
            m_lSystem.pushCurrentState();
            break;
        case ']':
            m_lSystem.popCurrentState();
            break;
        default: break;
        }
    }
}

void Tree::generateBranch(LSystemState currentState){
    float length = currentState.currentLength;
    glm::mat4 translatePosition = currentState.translateMatrix;
    glm::vec3 axisOfBranch = normalize(currentState.axisFacing);
    glm::vec3 yAxis = glm::vec3(0, 1.0f, 0);
    float thetaRotation = acos(glm::dot(axisOfBranch, yAxis) /
                               (glm::length(axisOfBranch) * glm::length(yAxis)));
    glm::vec3 axisOfRotation = glm::cross(yAxis, axisOfBranch);
    glm::mat4 rotate = glm::mat4(1.0f);
    if(glm::length(axisOfRotation) != 0.0){
        rotate = glm::rotate(thetaRotation, axisOfRotation);
    }
    //shifts the base of the cylinder by half of its height
    glm::mat4 translate = glm::translate((length * 0.5f)*axisOfBranch)*translatePosition;
    glm::mat4 scale = glm::scale(glm::vec3(0.2, length, 0.2));
    Cylinder cylinder = Cylinder();
    cylinder.setCtm(translate*rotate*scale);
    cylinder.setColor(glm::vec3(0.627f, 0.322f, 0.176f));
    cylinder.updateParams(5, 5);
    std::vector<float> newData = cylinder.generateShape();
    m_vertexData.insert(m_vertexData.end(), newData.begin(), newData.end());

    LSystemState newState = {length - 0.2f, glm::translate(length*axisOfBranch)*translatePosition, currentState.axisFacing};
    m_lSystem.updateCurrentState(newState);


}
void Tree::generateRecursively(){
    Cylinder cylinder = Cylinder();
    //start off with a "trunk" of height 3.0.
    glm::mat4 scale = glm::scale(glm::vec3(0.06, 2.0, 0.06));
    glm::mat4 totalTranslate = glm::mat4(1.0f);
    float currentHeight = 2.0f;
    cylinder.setCtm(scale);
    cylinder.updateParams(9, 9);
    m_vertexData = cylinder.generateShape();
    generateSubtree(currentHeight, totalTranslate, glm::radians(0.0f), glm::vec3(0, 1.0f, 0));
    generateSubtree(currentHeight, totalTranslate, glm::radians(30.0f), glm::vec3(0, 1.0f,  0));
    generateSubtree(currentHeight, totalTranslate, glm::radians(-30.0f), glm::vec3(0, 1.0f, 0));
    generateSubtree(currentHeight, totalTranslate, glm::radians(60.0f), glm::vec3 (0, 1.0f, 0));
    generateSubtree(currentHeight, totalTranslate, glm::radians(-60.0f), glm::vec3(0, 1.0f, 0));
    generateSubtree(currentHeight, totalTranslate, glm::radians(-90.0f), glm::vec3(0, 1.0f, 0));
    generateSubtree(currentHeight, totalTranslate, glm::radians(90.0f), glm::vec3(0, 1.0f, 0));

}

void Tree::generateSubtree(float currentHeight, glm::mat4 totalTranslate, float angleOfRotation, glm::vec3 axis){
    if(currentHeight > 0.1f){
        float newHeight = 0.6f * currentHeight;
        glm::mat4 translate = totalTranslate*glm::translate((currentHeight / 2.0f)*axis);
        //horizontal tilt
        glm::mat4 rotate = glm::rotate(angleOfRotation, glm::vec3(1.0, 0, 0));
        //random rotate
        glm::mat4 rotateAround = glm::rotate(glm::radians(float(rand()%90)), glm::vec3(axis));

        glm::mat4 scale = glm::scale(glm::vec3(0.06, newHeight, 0.06));
        glm::mat4 ctm = translate*rotate*rotateAround*scale; //this is the ctm that centers the origin at the top of the previous branch
        glm::vec3 axisOfRotation = normalize(rotate*rotateAround * glm::vec4(0, 1.0f, 0, 0));
        glm::mat4 finalTranslate = glm::translate(axisOfRotation*(newHeight/2.0f));
        Cylinder cylinder = Cylinder();
        cylinder.setCtm(finalTranslate*translate*rotate*rotateAround*scale);
        cylinder.updateParams(9, 9);
        std::vector<float> newData = cylinder.generateShape();
        m_vertexData.insert(m_vertexData.end(), newData.begin(), newData.end());

        generateSubtree(newHeight, finalTranslate*translate, angleOfRotation+glm::radians(30.0f), axisOfRotation);
        generateSubtree(newHeight, finalTranslate*translate, angleOfRotation-glm::radians(30.0f), axisOfRotation);
        generateSubtree(newHeight, finalTranslate*translate, angleOfRotation, axisOfRotation);
    }
}
