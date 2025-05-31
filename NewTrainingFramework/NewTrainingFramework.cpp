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
#include "ResourceManager.h"
#include "SceneManager.h"


#define PI 3.14


float angle, step = 0.001f;
float totalTime = 0.0f;


int Init(ESContext* esContext) {
	ResourceManager::GetInstance()->Init("../Resources/resourceManager.xml");
	SceneManager::GetInstance()->Init("../Resources/sceneManager.xml");
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return 0;
}


void Draw(ESContext* esContext) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SceneManager::GetInstance()->Draw();
	eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}


void Update ( ESContext *esContext, float deltaTime )
{
	SceneManager::GetInstance()->Update(deltaTime);
}

void Key(ESContext* esContext, unsigned char key, bool bIsPressed)
{
	Camera* myCamera = SceneManager::GetInstance()->getCamera();

	if (bIsPressed) {
		std::cout << "Key pressed: " << key << std::endl;
	}
	else {
		std::cout << "Key released: " << key << std::endl;
	}

	if (bIsPressed) {
		switch (key) {
			// Translație cameră
		case 'W': case 'w': myCamera->moveOz(-1); break; // Mișcare înainte
		case 'S': case 's': myCamera->moveOz(1); break;  // Mișcare înapoi
		case 'A': case 'a': myCamera->moveOx(-1); break; // Mișcare la stânga
		case 'D': case 'd': myCamera->moveOx(1); break;  // Mișcare la dreapta
		case 'Q': case 'q': myCamera->moveOy(1); break;  // Mișcare în sus
		case 'E': case 'e': myCamera->moveOy(-1); break; // Mișcare în jos

			// Rotire cameră
		case VK_UP: myCamera->rotateOx(-1); break;       // Rotire în sus
		case VK_DOWN: myCamera->rotateOx(1); break;      // Rotire în jos
		case VK_LEFT: myCamera->rotateOy(-1); break;     // Rotire la stânga
		case VK_RIGHT: myCamera->rotateOy(1); break;     // Rotire la dreapta

			// Rotire față de axa OZ (alegem tastele Z și X)
		case 'Z': case 'z': myCamera->rotateOz(-1); break; // Rotire negativă pe OZ
		case 'X': case 'x': myCamera->rotateOz(1); break;  // Rotire pozitivă pe OZ

		case 'P': myCamera->worldMatrix.print(); break;

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

