#include "stdafx.h"
#include "Model.h" 

Model::Model(ModelResource* resource)
    : mr(resource), vboId(0), iboId(0), wiredIboId(0), nrIndici(0), nrIndiciWired(0) {}

Model::~Model() {
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &iboId);
    glDeleteBuffers(1, &wiredIboId);
}

void Model::Load() {
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
    readNfg(mr->path, vertices, indices);

    nrIndici = indices.size();

    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::readNfg(const std::string& filePath, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open NFG file: " << filePath << std::endl;
		return;
	}

	std::string line;
	int vertexCount = 0, indexCount = 0;
	bool readVertex = false, readIndex = false;

	while (std::getline(file, line)) {
		std::istringstream iss(line);

		if (line.find("NrVertices") != std::string::npos) {
			iss.ignore(11);
			iss >> vertexCount;
			readVertex = true;
		}
		else if (line.find("NrIndices") != std::string::npos) {
			iss.ignore(10);
			iss >> indexCount;
			readIndex = true;
		}
		if (readVertex) {
			for (int i = 0; i < vertexCount; ++i) {
				std::getline(file, line);
				Vertex v;
				sscanf(line.c_str(),
					"   %*d. pos:[%f,%f,%f]; norm:[%f,%f,%f]; binorm:[%f,%f,%f]; tgt:[%f,%f,%f]; uv:[%f,%f];",
					&v.pos.x, &v.pos.y, &v.pos.z,
					&v.normal.x, &v.normal.y, &v.normal.z,
					&v.binormal.x, &v.binormal.y, &v.binormal.z,
					&v.tangent.x, &v.tangent.y, &v.tangent.z,
					&v.uv.x, &v.uv.y);
				vertices.push_back(v);
			}
			readVertex = false;
		}
		else if (readIndex) {
			for (int i = 0; i < indexCount / 3; ++i) {
				std::getline(file, line);
				unsigned short idx[3];
				sscanf(line.c_str(), "   %*d.    %hu,    %hu,    %hu", &idx[0], &idx[1], &idx[2]);
				indices.push_back(idx[0]);
				indices.push_back(idx[1]);
				indices.push_back(idx[2]);
			}
			readIndex = false;
		}
	}

	file.close();
	std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices from " << filePath << std::endl;
}
