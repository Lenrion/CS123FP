#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    renderData.cameraData = fileReader.getCameraData();
    renderData.globalData = fileReader.getGlobalData();


    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    renderData.shapes.clear();
    renderData.lights.clear();
    dfsHelper(fileReader.getRootNode(), renderData, glm::mat4(1.f));


    return true;
}

void SceneParser::dfsHelper(SceneNode* rootNode, RenderData &renderData, glm::mat4 ctm){
    for(int i = 0; i < rootNode->transformations.size(); i++){
        switch(rootNode->transformations[i]->type){
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = ctm * rootNode->transformations[i]->matrix;
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm =  ctm *glm::scale(rootNode->transformations[i]->scale);
            break;
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = ctm * glm::translate(rootNode->transformations[i]->translate);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            ctm = ctm * glm::rotate(rootNode->transformations[i]->angle, rootNode->transformations[i]->rotate);
            break;
        default:
            break;
        }

    }
    for(int i = 0; i < rootNode->primitives.size(); i++){
        RenderShapeData rsData = RenderShapeData{*rootNode->primitives[i], ctm};
        renderData.shapes.push_back(rsData);
    }
    for(int i = 0; i < rootNode->lights.size(); i++){
        SceneLight light = *rootNode->lights[i];
        SceneLightData sldata {light.id,light.type, light.color, light.function, ctm * glm::vec4{0,0,0,1},
                              ctm * light.dir, light.penumbra, light.angle, light.width, light.height};
        renderData.lights.push_back(sldata);
    }

    for(int i = 0; i < rootNode->children.size(); i++){
        dfsHelper(rootNode->children[i], renderData, ctm);
    }


}
