#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

inline bool floatCompare(float f1, float f2){
    return (fabs(f1 - f2) < 0.01);
}
struct LSystemState {
    float currentLength = 2.0f;
    glm::mat4 translateMatrix = glm::mat4(1.0f);
    glm::vec3 axisFacing = glm::vec3(0.0, 1.0f, 0.0); //the initial state is a branch facing up.
    int branchNumber = 1;
};
//};

//struct LSystemStateHash {
//    std::size_t operator()(const LSystemState& s) const {
//        std::size_t h1 = std::hash<float>()(s.currentLength);
//        std::size_t h2 = std::hash<float>()(s.axisFacing.x);
//        std::size_t h3 = std::hash<float>()(s.axisFacing.y);
//        std::size_t h4 = std::hash<float>()(s.axisFacing.z);
//        std::size_t h5 = std::hash<float>()(s.translateMatrix[3][0]);
//        std::size_t h6 = std::hash<float>()(s.translateMatrix[3][1]);
//        std::size_t h7 = std::hash<float>()(s.translateMatrix[3][2]);
//        return h1 + h2 + h3 + h4 + h5 + h6 + h7;
//    }
//};

struct LSystemStateNode {
    LSystemState state;
    std::vector<LSystemStateNode> children;
};
class LSystem
{
public:
    LSystem(){};
    LSystemState getCurrentState(){ return m_currentState; };
    void updateCurrentState(LSystemState newState){m_currentState = newState; };
    void pushCurrentState();
    void popCurrentState();
    std::string getSentence(){ return m_sentence; };
    void rotateFacing(float theta);
    void applyRules(int numGenerations);
    void generatedBranch(bool generated){ m_generated = generated; };
    std::vector<LSystemState> getTerminatedStates(){ return m_terminatedBranchStates; };

private:
    std::stack<LSystemState> m_stateStack;
    std::vector<LSystemState> m_terminatedBranchStates;
    std::unordered_map<std::string, std::string> m_rules = {
        {"F", "F[++F][--F]"}
    };
    std::string m_axiom = "F";
    std::string m_sentence = m_axiom;
    LSystemState m_currentState = LSystemState{};
    std::string getRule(std::string character);
    bool m_generated = false;

};
