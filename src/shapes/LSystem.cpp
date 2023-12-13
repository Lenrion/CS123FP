#include "glm/gtx/transform.hpp"
#include <src/shapes/lsystem.h>

void LSystem::rotateFacing(float theta){
    //there's a chance that these could all be zero.
    glm::vec3 randomAxis = glm::vec3(0, 0, 0);
    int randomAgain = rand()%3;
    randomAxis[randomAgain] = 1.0f;

    glm::mat4 rotate = glm::rotate(theta, randomAxis);
    m_currentState.axisFacing = rotate*glm::vec4(m_currentState.axisFacing, 0.0);
}
void LSystem::pushCurrentState(){
    m_generated = false;
    m_stateStack.push(m_currentState);
}

void LSystem::popCurrentState(){
    if(m_generated){
        m_terminatedBranchStates.push_back(m_currentState);
        m_generated = false;
    }
    m_currentState = m_stateStack.top();
    m_stateStack.pop();
}

void LSystem::applyRules(int numGenerations){
    std::string sentence = m_axiom;
    std::string newSentence = "";
    for(int g = 0; g < numGenerations-1; g++){
        for(char c : sentence){
            std::string asString = "";
            asString += c;
            if(m_rules.contains(asString)){
                newSentence += m_rules[asString];
            } else {
                newSentence += asString;
            }
        }
        sentence = newSentence;
        newSentence = "";
    }
    m_sentence = sentence;
}
