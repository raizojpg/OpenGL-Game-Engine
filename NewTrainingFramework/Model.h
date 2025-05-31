#pragma once

#include "../Utilities/utilities.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "Vertex.h"

struct ModelResource {
    int id;
    std::string path;
};

class Model {
public:
    ModelResource* mr;
    GLuint vboId, iboId, wiredIboId;
    int nrIndexes, nrIndexesWired;

    Model(ModelResource* resource);
    ~Model();

    void Load();
    void readNfg(const std::string& filePath, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices);
};