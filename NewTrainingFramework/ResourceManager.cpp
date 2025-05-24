#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager* ResourceManager::GetInstance() {
    if (!instance) {
        instance = new ResourceManager();
    }
    return instance;
}

void ResourceManager::Init() {
    std::cout << "ResourceManager initialized." << std::endl;
}

void ResourceManager::LoadConfig(const std::string& filePath) {
    try {
        // Load and parse the XML file
        rapidxml::file<> file(filePath.c_str());
        rapidxml::xml_document<> doc;
        doc.parse<0>(file.data());

        // Get the root node
        rapidxml::xml_node<>* root = doc.first_node("resourceManager");
        if (!root) {
            throw std::runtime_error("Missing 'resourceManager' root node in config file.");
        }

        // Parse models
        rapidxml::xml_node<>* modelsNode = root->first_node("models");
        if (modelsNode) {
            for (rapidxml::xml_node<>* folderNode = modelsNode->first_node("folder"); folderNode; folderNode = folderNode->next_sibling("folder")) {
                std::string folderPath = folderNode->first_attribute("path")->value();
                for (rapidxml::xml_node<>* modelNode = folderNode->first_node("model"); modelNode; modelNode = modelNode->next_sibling("model")) {
                    int id = std::stoi(modelNode->first_attribute("id")->value());
                    std::string fileName = modelNode->first_node("file")->value();

                    ModelResource* mr = new ModelResource();
                    mr->id = id;
                    mr->path = folderPath + fileName;
                    modelResources[id] = mr;
                }
            }
        }

        // Parse shaders
        rapidxml::xml_node<>* shadersNode = root->first_node("shaders");
        if (shadersNode) {
            for (rapidxml::xml_node<>* folderNode = shadersNode->first_node("folder"); folderNode; folderNode = folderNode->next_sibling("folder")) {
                std::string folderPath = folderNode->first_attribute("path")->value();
                for (rapidxml::xml_node<>* shaderNode = folderNode->first_node("shader"); shaderNode; shaderNode = shaderNode->next_sibling("shader")) {
                    int id = std::stoi(shaderNode->first_attribute("id")->value());
                    std::string vertexShader = shaderNode->first_node("vs")->value();
                    std::string fragmentShader = shaderNode->first_node("fs")->value();

                    ShaderResource* sr = new ShaderResource();
                    sr->id = id;
                    sr->vertexShaderPath = folderPath + vertexShader;
                    sr->fragmentShaderPath = folderPath + fragmentShader;
                    shaderResources[id] = sr;
                }
            }
        }

        // Parse textures
        rapidxml::xml_node<>* texturesNode = root->first_node("textures");
        if (texturesNode) {
            for (rapidxml::xml_node<>* folderNode = texturesNode->first_node("folder"); folderNode; folderNode = folderNode->next_sibling("folder")) {
                std::string folderPath = folderNode->first_attribute("path")->value();
                for (rapidxml::xml_node<>* textureNode = folderNode->first_node("texture"); textureNode; textureNode = textureNode->next_sibling("texture")) {
                    int id = std::stoi(textureNode->first_attribute("id")->value());
                    std::string fileName = textureNode->first_node("file")->value();

                    TextureResource* tr = new TextureResource();
                    tr->id = id;
                    tr->path = folderPath + fileName;
                    textureResources[id] = tr;
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
    }
}

Model* ResourceManager::LoadModel(int id) {
    if (loadedModels.find(id) != loadedModels.end()) {
        return loadedModels[id];
    }

    if (modelResources.find(id) == modelResources.end()) {
        std::cerr << "Model with ID " << id << " not found!" << std::endl;
        return nullptr;
    }

    Model* model = new Model(modelResources[id]);
    model->Load();
    loadedModels[id] = model;
    return model;
}

Shader* ResourceManager::LoadShader(int id) {
    if (loadedShaders.find(id) != loadedShaders.end()) {
        return loadedShaders[id];
    }

    if (shaderResources.find(id) == shaderResources.end()) {
        std::cerr << "Shader with ID " << id << " not found!" << std::endl;
        return nullptr;
    }

    Shader* shader = new Shader(shaderResources[id]);
    shader->Load();
    loadedShaders[id] = shader;
    return shader;
}

Texture* ResourceManager::LoadTexture(int id) {
    if (loadedTextures.find(id) != loadedTextures.end()) {
        return loadedTextures[id];
    }

    if (textureResources.find(id) == textureResources.end()) {
        std::cerr << "Texture with ID " << id << " not found!" << std::endl;
        return nullptr;
    }

    Texture* texture = new Texture(textureResources[id]);
    texture->Load();
    loadedTextures[id] = texture;
    return texture;
}

void ResourceManager::FreeResources() {
    for (auto& pair : modelResources) {
        delete pair.second;
    }
    modelResources.clear();

    for (auto& pair : shaderResources) {
        delete pair.second;
    }
    shaderResources.clear();

    for (auto& pair : textureResources) {
        delete pair.second;
    }
    textureResources.clear();

    for (auto& pair : loadedModels) {
        delete pair.second;
    }
    loadedModels.clear();

    for (auto& pair : loadedShaders) {
        delete pair.second;
    }
    loadedShaders.clear();

    for (auto& pair : loadedTextures) {
        delete pair.second;
    }
    loadedTextures.clear();

    std::cout << "Resources freed." << std::endl;
}

ResourceManager::~ResourceManager() {
    FreeResources();
}