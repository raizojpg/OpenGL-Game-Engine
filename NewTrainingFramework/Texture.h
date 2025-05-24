#pragma once

#include "../Utilities/utilities.h"
#include <iostream>

struct TextureResource {
    int id;
    std::string path;
};

class Texture {
public:
    TextureResource* tr;
    GLuint textureId;

    Texture(TextureResource* resource);
    ~Texture();

    void Load();
};