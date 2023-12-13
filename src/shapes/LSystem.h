#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <stack>
#include <unordered_map>

struct LSystemState {
    float currentLength = 1.0f;
    glm::mat4 translateMatrix = glm::mat4(1.0f);
    glm::vec3 axisFacing = glm::vec3(0.0, 1.0f, 0.0); //the initial state is a branch facing up.
};

class LSystem
{
public:
    LSystem(){};
    LSystemState getCurrentState(){ return m_currentState; };
    void updateCurrentState(LSystemState newState){m_currentState = newState; };
    void pushCurrentState(){ m_stateStack.push(m_currentState); };
    void popCurrentState();
    std::string getSentence(){ return m_sentence; };
    void rotateFacing(float theta);
    void applyRules(int numGenerations);

private:
    std::stack<LSystemState> m_stateStack;
    std::unordered_map<std::string, std::string> m_rules = {
        {"F", "F[++F[+F]-F][--F[-F]+F]"}
    };
    std::string m_axiom = "F";
    std::string m_sentence = m_axiom;
    LSystemState m_currentState = LSystemState{};
    std::string getRule(std::string character);

};
