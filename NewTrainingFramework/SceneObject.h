#pragma once

#include "../Utilities/utilities.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include <vector>

class SceneObject {
public:
    int id;
    std::string name;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    Model* model;
    Shader* shader;
    std::vector<Texture*> textures;
    bool depthTest;
    bool wiredFormat;
    std::string type;

    SceneObject();
    virtual void Draw(Camera* activeCamera);
    virtual void Update(float deltaTime);
    virtual ~SceneObject();

protected:
    virtual void sendCommonData(Camera* activeCamera);
    virtual void sendSpecificData();
};