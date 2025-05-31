#include "stdafx.h"
#include "SceneManager.h"
#include <iostream>
#include <stdexcept>

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance() {
    if (!instance) {
        instance = new SceneManager();
    }
    return instance;
}

SceneManager::SceneManager() : activeCamera(nullptr) {
    backgroundColor[0] = 0.0f; // R
    backgroundColor[1] = 0.0f; // G
    backgroundColor[2] = 0.0f; // B
    backgroundColor[3] = 1.0f; // A
}

void SceneManager::Init(const std::string& configFilePath) {
    try {
        rapidxml::file<> file(configFilePath.c_str());
        rapidxml::xml_document<> doc;
        doc.parse<0>(file.data());

        rapidxml::xml_node<>* root = doc.first_node("sceneManager");
        if (!root) {
            throw std::runtime_error("Missing 'sceneManager' root node in config file.");
        }

        ParseGameName(root);
        ParseDefaultScreenSize(root);
        ParseBackgroundColor(root);
        ParseControls(root);
        ParseCameras(root);
        ParseObjects(root);
        ParseDebugSettings(root);
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing scene: " << e.what() << std::endl;
    }
}

void SceneManager::ParseGameName(rapidxml::xml_node<>* root) {
    rapidxml::xml_node<>* gameNameNode = root->first_node("gameName");
    if (gameNameNode) {
        Globals::gameName = gameNameNode->value();
    }
}

void SceneManager::ParseDefaultScreenSize(rapidxml::xml_node<>* root) {
    //rapidxml::xml_node<>* screenSizeNode = root->first_node("defaultScreenSize");
    //if (screenSizeNode) {
    //    Globals::screenWidth = std::stoi(screenSizeNode->first_node("width")->value());
    //    Globals::screenHeight = std::stoi(screenSizeNode->first_node("height")->value());
    //}
}

void SceneManager::ParseBackgroundColor(rapidxml::xml_node<>* root) {
    rapidxml::xml_node<>* backgroundColorNode = root->first_node("backgroundColor");
    if (backgroundColorNode) {
        backgroundColor[0] = std::stof(backgroundColorNode->first_node("r")->value());
        backgroundColor[1] = std::stof(backgroundColorNode->first_node("g")->value());
        backgroundColor[2] = std::stof(backgroundColorNode->first_node("b")->value());
    }
}

void SceneManager::ParseControls(rapidxml::xml_node<>* root) {
    //rapidxml::xml_node<>* controlsNode = root->first_node("controls");
    //if (controlsNode) {
    //    for (rapidxml::xml_node<>* controlNode = controlsNode->first_node("control"); controlNode; controlNode = controlNode->next_sibling("control")) {
    //        std::string key = controlNode->first_node("key")->value();
    //        std::string action = controlNode->first_node("action")->value();
    //        controls[key] = action;
    //    }
    //}
}

void SceneManager::ParseCameras(rapidxml::xml_node<>* root) {
    rapidxml::xml_node<>* camerasNode = root->first_node("cameras");
    if (camerasNode) {
        for (rapidxml::xml_node<>* cameraNode = camerasNode->first_node("camera"); cameraNode; cameraNode = cameraNode->next_sibling("camera")) {
            Camera* camera = new Camera();
            camera->id = std::stoi(cameraNode->first_attribute("id")->value());
            camera->position = ParseVector3(cameraNode->first_node("position"));
            camera->target = ParseVector3(cameraNode->first_node("target"));
            camera->up = ParseVector3(cameraNode->first_node("up"));
            camera->moveSpeed = std::stof(cameraNode->first_node("translationSpeed")->value());
            camera->rotateSpeed = std::stof(cameraNode->first_node("rotationSpeed")->value());
            camera->fov = std::stof(cameraNode->first_node("fov")->value());
            camera->nearPlane = std::stof(cameraNode->first_node("near")->value());
            camera->farPlane = std::stof(cameraNode->first_node("far")->value());
            camera->updatePerspective();
            camera->updateWorldView();
            cameras.push_back(camera);
            std::cout << "Loaded camera with id " << camera->id<<"\n";
        }

        rapidxml::xml_node<>* activeCameraNode = root->first_node("activeCamera");
        if (activeCameraNode) {
            int activeCameraId = std::stoi(activeCameraNode->value());
            for (Camera* camera : cameras) {
                if (camera->id == activeCameraId) {
                    activeCamera = camera;
                    break;
                }
            }
        }
    }
}

void SceneManager::ParseObjects(rapidxml::xml_node<>* root) {
    rapidxml::xml_node<>* objectsNode = root->first_node("objects");
    if (objectsNode) {
        for (rapidxml::xml_node<>* objectNode = objectsNode->first_node("object"); objectNode; objectNode = objectNode->next_sibling("object")) {
            SceneObject* object = new SceneObject();

            // Parse object ID
            rapidxml::xml_attribute<>* idAttribute = objectNode->first_attribute("id");
            if (idAttribute) {
                object->id = std::stoi(idAttribute->value());
                std::cout << "\nReading object with id " << object->id << "\n";
            }
            else {
                std::cerr << "Warning: Missing 'id' attribute in <object> node." << std::endl;
                delete object;
                continue;
            }

            // Parse name
            rapidxml::xml_node<>* nameNode = objectNode->first_node("name");
            if (nameNode) {
                object->name = nameNode->value();
            }
            else {
                std::cerr << "Warning: Missing <name> node in <object> with ID " << object->id << "." << std::endl;
            }

            // Parse type
            rapidxml::xml_node<>* typeNode = objectNode->first_node("type");
            if (typeNode) {
                typeNode->value();
            }
            else {
                std::cerr << "Warning: Missing <type> node in <object> with ID " << object->id << "." << std::endl;
            }

            // Parse position, rotation, and scale
            object->position = ParseVector3(objectNode->first_node("position"));
            object->rotation = ParseVector3(objectNode->first_node("rotation"));
            object->scale = ParseVector3(objectNode->first_node("scale"));

            // Parse shader
            rapidxml::xml_node<>* shaderNode = objectNode->first_node("shader");
            if (shaderNode) {
                int shaderId = std::stoi(shaderNode->value());
                Shader* shader = ResourceManager::GetInstance()->LoadShader(shaderId);
                if (shader) {
                    object->shader = shader;
                }
                else {
                    std::cerr << "Warning: Shader with ID " << shaderId << " not found in ResourceManager." << std::endl;
                }
            }
            else {
                std::cerr << "Warning: Missing <shader> node in <object> with ID " << object->id << "." << std::endl;
            }

            // Parse model
            rapidxml::xml_node<>* modelNode = objectNode->first_node("model");
            if (modelNode) {
                int modelId = std::stoi(modelNode->value());
                Model* model = ResourceManager::GetInstance()->LoadModel(modelId);
                if (model) {
                    object->model = model;
                }
                else {
                    std::cerr << "Warning: Model with ID " << modelId << " not found in ResourceManager." << std::endl;
                }
            }
            else {
                std::cerr << "Warning: Missing <model> node in <object> with ID " << object->id << "." << std::endl;
            }

            // Parse textures
            rapidxml::xml_node<>* texturesNode = objectNode->first_node("textures");
            if (texturesNode) {
                for (rapidxml::xml_node<>* textureNode = texturesNode->first_node("texture"); textureNode; textureNode = textureNode->next_sibling("texture")) {
                    rapidxml::xml_attribute<>* textureIdAttribute = textureNode->first_attribute("id");
                    if (textureIdAttribute) {
                        int textureId = std::stoi(textureIdAttribute->value());
                        Texture* texture = ResourceManager::GetInstance()->LoadTexture(textureId);
                        if (texture) {
                            object->textures.push_back(texture);
                        }
                        else {
                            std::cerr << "Warning: Texture with ID " << textureId << " not found in ResourceManager." << std::endl;
                        }
                    }
                    else {
                        std::cerr << "Warning: Missing 'id' attribute in <texture> node." << std::endl;
                    }
                }
            }

            // Add the parsed object to the scene
            sceneObjects.push_back(object);
        }
    }
    else {
        std::cerr << "Warning: Missing <objects> node in the scene configuration." << std::endl;
    }
}

void SceneManager::ParseDebugSettings(rapidxml::xml_node<>* root) {
    //rapidxml::xml_node<>* debugSettingsNode = root->first_node("debugSettings");
    //if (debugSettingsNode) {
    //    rapidxml::xml_node<>* objectAxesNode = debugSettingsNode->first_node("objectAxes");
    //    if (objectAxesNode) {
    //        objectAxesColors["OX"] = ParseVector3(objectAxesNode->first_node("OXColor"));
    //        objectAxesColors["OY"] = ParseVector3(objectAxesNode->first_node("OYColor"));
    //        objectAxesColors["OZ"] = ParseVector3(objectAxesNode->first_node("OZColor"));
    //    }
    //}
}

Vector3 SceneManager::ParseVector3(rapidxml::xml_node<>* node) {
    return Vector3(
        std::stof(node->first_node("x")->value()),
        std::stof(node->first_node("y")->value()),
        std::stof(node->first_node("z")->value())
    );
}


void SceneManager::Draw() {
    for (SceneObject* object : sceneObjects) {
        object->Draw(activeCamera);
    }
}

void SceneManager::Update(float deltaTime) {
    if (activeCamera) {
        activeCamera->setDeltaTime(deltaTime);
        activeCamera->updateWorldView();
    }
    
    for (SceneObject* object : sceneObjects) {
        object->Update(deltaTime);
    }
}

void SceneManager::FreeResources() {
    for (Camera* camera : cameras) {
        delete camera;
    }
    cameras.clear();

    for (SceneObject* object : sceneObjects) {
        delete object;
    }
    sceneObjects.clear();
}

SceneManager::~SceneManager() {
    FreeResources();
}