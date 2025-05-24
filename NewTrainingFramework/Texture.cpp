#include "stdafx.h"
#include "Texture.h" 

Texture::Texture(TextureResource* resource)
    : tr(resource), textureId(0) {}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

void Texture::Load() {
    int width, height, bpp;
    char* pixelArray = LoadTGA(tr->path.c_str(), &width, &height, &bpp);

	pixelArray = LoadTGA("../Resources/Textures/Croco.tga", &width, &height, &bpp);

	if (pixelArray == nullptr) {
		std::cerr << "Failed to load texture!" << std::endl;
	}
	else {
		std::cout << "Texture loaded successfully: " << width << "x" << height << ", " << bpp << " bpp" << std::endl;
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(
		GL_TEXTURE_2D,          // Tipul texturii
		0,                      // Nivelul de mipmap
		(bpp == 24) ? GL_RGB : GL_RGBA, // Formatul intern al texturii
		width,                  // L??imea imaginii
		height,                 // În?l?imea imaginii
		0,                      // Border
		(bpp == 24) ? GL_RGB : GL_RGBA, // Formatul datelor pixelilor
		GL_UNSIGNED_BYTE,       // Modul de reprezentare a culorilor
		pixelArray              // Datele imaginii
	);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pixelArray;
}
