// NewTrainingFramework.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"
#include "../Utilities/utilities.h" // if you use STL, please include this line AFTER all other include
#include "Vertex.h"
#include "Shaders.h"
#include <conio.h>
#include "Globals.h"
#include "Camera.h"
#include "MouseEnums.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


#define PI 3.14

GLuint vboId;
GLuint lineVboId;

Shaders myShaders;
Shaders lineShaders;

Camera myCamera;

GLuint modelVboId;
GLuint modelIboId;

Shaders modelShaders;

GLuint idTexture;

float angle, step = 0.001f;
float totalTime = 0.0f;

std::vector<Vertex> verticesData;
std::vector<unsigned short> indicesData;

//int Init(ESContext* esContext)
//{
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//	Vertex verticesData[6];
//	verticesData[0].pos = Vector3(-0.5f, 0.5f, 0.0f); verticesData[0].color = Vector3(1.0f, 0.0f, 0.0f); // Roșu
//	verticesData[1].pos = Vector3(-0.5f, -0.5f, 0.0f); verticesData[1].color = Vector3(0.0f, 1.0f, 0.0f); // Verde
//	verticesData[2].pos = Vector3(0.5f, -0.5f, 0.0f); verticesData[2].color = Vector3(0.0f, 0.0f, 1.0f); // Albastru
//	verticesData[3].pos = Vector3(-0.5f, 0.5f, 0.0f); verticesData[3].color = Vector3(1.0f, 0.0f, 0.0f); // Roșu
//	verticesData[4].pos = Vector3(0.5f, 0.5f, 0.0f); verticesData[4].color = Vector3(1.0f, 1.0f, 0.0f); // Galben
//	verticesData[5].pos = Vector3(0.5f, -0.5f, 0.0f); verticesData[5].color = Vector3(0.0f, 0.0f, 1.0f); // Albastru
//
//	glGenBuffers(1, &vboId);
//	glBindBuffer(GL_ARRAY_BUFFER, vboId);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData), verticesData, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	Vector3 lineVerticesData[2];
//	lineVerticesData[0] = Vector3(0.0f, 1.0f, 0.0f);
//	lineVerticesData[1] = Vector3(0.0f, -1.0f, 0.0f);
//
//	glGenBuffers(1, &lineVboId);
//	glBindBuffer(GL_ARRAY_BUFFER, lineVboId);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerticesData), lineVerticesData, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	lineShaders.Init("../Resources/Shaders/LineShaderVS.vs", "../Resources/Shaders/LineShaderFS.fs");
//	return myShaders.Init("../Resources/Shaders/TriangleShaderVS.vs", "../Resources/Shaders/TriangleShaderFS.fs");
//}


void readNfg(const std::string& filePath, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices) {
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

void parseXmlNode(rapidxml::xml_node<>* node, int depth = 0) {
	if (!node || std::strlen(node->name()) == 0) {
		return; 
	}

	std::string indent(depth * 4, ' ');
	std::cout << indent << "*NODE: " << node->name();

	rapidxml::xml_attribute<>* attr = node->first_attribute();
	if (attr) {
		std::cout << "\t\t has with attributes ";
		while (attr) {
			std::cout << attr->name() << "=" << attr->value();
			attr = attr->next_attribute();
			if (attr) std::cout << ", ";
		}
	}
	else {
		std::cout << "\t\t without attributes";
	}
	std::cout << std::endl;

	if (node->value() && std::strlen(node->value()) > 0) {
		std::cout << indent << " -> with content: " << node->value() << std::endl;
	}

	rapidxml::xml_node<>* child = node->first_node();
	if (child && std::strlen(child->name()) != 0) {
		std::cout << indent << " -> with children:" << std::endl;
		while (child) {
			parseXmlNode(child, depth + 1);
			child = child->next_sibling();
		}
	}
}

void printXML() {
	try {
		rapidxml::file<> file("../Resources/doc.xml");
		char* buffer = file.data();

		rapidxml::xml_document<> doc;
		doc.parse<0>(buffer);

		rapidxml::xml_node<>* root = doc.first_node();
		if (root) {
			std::cout << "Root: " << root->name() << std::endl;
			parseXmlNode(root);
		}
		else {
			std::cerr << "Failed to find root node in XML file." << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing XML: " << e.what() << std::endl;
	}
}

int Init(ESContext* esContext)
{
	printXML();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	readNfg("../Resources/Models/Croco.nfg", verticesData, indicesData);

	glGenBuffers(1, &modelVboId);
	glBindBuffer(GL_ARRAY_BUFFER, modelVboId);
	glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(Vertex), verticesData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &modelIboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesData.size() * sizeof(unsigned short), indicesData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	int width, height, bpp;
	char* pixelArray = nullptr;

	pixelArray = LoadTGA("../Resources/Textures/Croco.tga", &width, &height, &bpp);

	if (pixelArray == nullptr) {
		std::cerr << "Failed to load texture!" << std::endl;
	}
	else {
		std::cout << "Texture loaded successfully: " << width << "x" << height << ", " << bpp << " bpp" << std::endl;
	}

	glGenTextures(1, &idTexture); 
	glBindTexture(GL_TEXTURE_2D, idTexture); 

	glTexImage2D(
		GL_TEXTURE_2D,          // Tipul texturii
		0,                      // Nivelul de mipmap
		(bpp == 24) ? GL_RGB : GL_RGBA, // Formatul intern al texturii
		width,                  // Lățimea imaginii
		height,                 // Înălțimea imaginii
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


	return modelShaders.Init("../Resources/Shaders/ModelShaderVS.vs", "../Resources/Shaders/ModelShaderFS.fs");
}

//void Draw ( ESContext *esContext )
//{
//
//	Matrix mRotation;
//	mRotation.SetRotationZ(angle);
//
//	Matrix MVP;
//	MVP.SetIdentity();
//	MVP = mRotation * myCamera.viewMatrix * myCamera.perspectiveMatrix;
//
//
//
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glUseProgram(myShaders.program);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vboId);
//
//	
//	if(myShaders.positionAttribute != -1)
//	{
//		glEnableVertexAttribArray(myShaders.positionAttribute);
//		glVertexAttribPointer(myShaders.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	}
//
//	if (myShaders.colorAttribute != -1)
//	{
//		glEnableVertexAttribArray(myShaders.colorAttribute);
//		glVertexAttribPointer(myShaders.colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3)));
//	}
//	if (myShaders.matrixUniform != -1) {
//		glUniformMatrix4fv(myShaders.matrixUniform, 1, GL_FALSE, (float*)MVP.m);
//		//MVP.print();
//	}
//
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//
//
//	glUseProgram(lineShaders.program);
//	
//	glBindBuffer(GL_ARRAY_BUFFER, lineVboId);
//
//	if (lineShaders.positionAttribute != -1)
//	{
//		glEnableVertexAttribArray(lineShaders.positionAttribute);
//		glVertexAttribPointer(lineShaders.positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
//	}
//
//	glDrawArrays(GL_LINES, 0, 2);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
//}

void Draw(ESContext* esContext)
{
	Matrix mRotation;
	mRotation.SetRotationY(angle);

	Matrix MVP;
	MVP.SetIdentity();
	MVP = mRotation * myCamera.viewMatrix * myCamera.perspectiveMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(modelShaders.program);

	if (modelShaders.matrixUniform != -1) {
		glUniformMatrix4fv(modelShaders.matrixUniform, 1, GL_FALSE, (float*)MVP.m);
	}

	if (modelShaders.textureUniform != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, idTexture);
		glUniform1i(modelShaders.textureUniform, 0);
	}


	glBindBuffer(GL_ARRAY_BUFFER, modelVboId);

	if (modelShaders.positionAttribute != -1) {
		glEnableVertexAttribArray(modelShaders.positionAttribute);
		glVertexAttribPointer(modelShaders.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}

	if (modelShaders.uvAttribute != -1) {
		glEnableVertexAttribArray(modelShaders.uvAttribute);
		glVertexAttribPointer(modelShaders.uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) * 4));
	}


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIboId);

	glDrawElements(GL_TRIANGLES, indicesData.size(), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}

void Update ( ESContext *esContext, float deltaTime )
{
	angle += step;
	if (angle >= 2 * PI) { angle -= 2 * PI; }
	if (angle <= -2 * PI) { angle += 2 * PI; }

	totalTime += deltaTime;

	if (totalTime >= Globals::frameTime) {
		totalTime -= Globals::frameTime; 

		myCamera.setDeltaTime(Globals::frameTime);
	}

}

void Key(ESContext* esContext, unsigned char key, bool bIsPressed)
{
	if (bIsPressed) {
		std::cout << "Key pressed: " << key << std::endl;
	}
	else {
		std::cout << "Key released: " << key << std::endl;
	}

	if (bIsPressed) {
		switch (key) {
			// Translație cameră
		case 'W': case 'w': myCamera.moveOz(-1); break; // Mișcare înainte
		case 'S': case 's': myCamera.moveOz(1); break;  // Mișcare înapoi
		case 'A': case 'a': myCamera.moveOx(-1); break; // Mișcare la stânga
		case 'D': case 'd': myCamera.moveOx(1); break;  // Mișcare la dreapta
		case 'Q': case 'q': myCamera.moveOy(1); break;  // Mișcare în sus
		case 'E': case 'e': myCamera.moveOy(-1); break; // Mișcare în jos

			// Rotire cameră
		case VK_UP: myCamera.rotateOx(-1); break;       // Rotire în sus
		case VK_DOWN: myCamera.rotateOx(1); break;      // Rotire în jos
		case VK_LEFT: myCamera.rotateOy(-1); break;     // Rotire la stânga
		case VK_RIGHT: myCamera.rotateOy(1); break;     // Rotire la dreapta

			// Rotire față de axa OZ (alegem tastele Z și X)
		case 'Z': case 'z': myCamera.rotateOz(-1); break; // Rotire negativă pe OZ
		case 'X': case 'x': myCamera.rotateOz(1); break;  // Rotire pozitivă pe OZ
		
		case 'P': myCamera.worldMatrix.print(); break;
	
		default:
			std::cout << "Unmapped key: " << key << std::endl;
			break;
		}
	}
}

void Mouse(ESContext* esContext, MouseButtons btn, MouseEvents event, int x, int y)
{
	if (event == MouseEvents::BTN_DOWN && btn == MouseButtons::LEFT)
	{
		if (x > Globals::screenWidth / 2)
		{
			step = (step > 0) ? -0.001f : step - 0.01f;
		}
		else
		{
			step = (step < 0) ? 0.001f : step + 0.01f;
		}

		std::cout << "Mouse click at (" << x << ", " << y << "), rotation speed: " << step << std::endl;
	}
}


void CleanUp()
{
	glDeleteBuffers(1, &vboId);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//identifying memory leaks
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	ESContext esContext;

    esInitContext ( &esContext );

	esCreateWindow ( &esContext, "Hello Triangle", Globals::screenWidth, Globals::screenHeight, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	if ( Init ( &esContext ) != 0 )
		return 0;

	esRegisterDrawFunc ( &esContext, Draw );
	esRegisterUpdateFunc ( &esContext, Update );
	esRegisterKeyFunc ( &esContext, Key);
	esRegisterMouseFunc(&esContext, Mouse);

	esMainLoop ( &esContext );

	//releasing OpenGL resources
	CleanUp();
	

	printf("Press any key...\n");
	_getch();

	
	return 0;
}

