#pragma once

#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"
#include "../Utilities/utilities.h"
#include <map>
#include <string>
#include <iostream>
#include "Model.h"
#include "Shader.h"
#include "Texture.h"

class ResourceManager {
private:
    static ResourceManager* instance;
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<int, ModelResource*> modelResources;
    std::map<int, ShaderResource*> shaderResources;
    std::map<int, TextureResource*> textureResources;

    std::map<int, Model*> loadedModels;
    std::map<int, Shader*> loadedShaders;
    std::map<int, Texture*> loadedTextures;

public:
    static ResourceManager* GetInstance();
    void Init();

    void LoadConfig(const std::string& filePath);
    Model* LoadModel(int id);
    Shader* LoadShader(int id);
    Texture* LoadTexture(int id);

    void FreeResources();

    ~ResourceManager();
};