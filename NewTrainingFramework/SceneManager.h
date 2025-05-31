#pragma once

#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"
#include "../Utilities/utilities.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "Camera.h"
#include "SceneObject.h"
#include "ResourceManager.h"
#include "Globals.h"

class SceneManager {
private:
    static SceneManager* instance;
    SceneManager();
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    std::vector<Camera*> cameras;
    Camera* activeCamera;
    std::vector<SceneObject*> sceneObjects;

    std::string windowTitle;
    float backgroundColor[4];

    void ParseGameName(rapidxml::xml_node<>* root);
    void ParseDefaultScreenSize(rapidxml::xml_node<>* root);
    void ParseBackgroundColor(rapidxml::xml_node<>* root);
    void ParseControls(rapidxml::xml_node<>* root);
    void ParseCameras(rapidxml::xml_node<>* root);
    void ParseObjects(rapidxml::xml_node<>* root);
    void ParseDebugSettings(rapidxml::xml_node<>* root);
    Vector3 ParseVector3(rapidxml::xml_node<>* node);

public:
    static SceneManager* GetInstance();

    void InitWindow(const std::string& configFilePath);
    void Init(const std::string& configFilePath);
    void Draw();
    void Update(float deltaTime);
    Camera* getCamera() { return activeCamera; }

    void FreeResources();
    ~SceneManager();
};