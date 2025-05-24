#pragma once

#include "../Utilities/utilities.h"
#include <iostream>

struct ShaderResource {
    int id;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
};

class Shader {
public:
    ShaderResource* sr;
    GLuint programId;
    GLint positionAttribute, colorAttribute, matrixUniform;
    GLuint textureUniform, uvAttribute;

    Shader(ShaderResource* resource);
    ~Shader();

    void Load();
};

