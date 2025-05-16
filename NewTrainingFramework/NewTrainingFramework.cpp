// NewTrainingFramework.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Utilities/utilities.h" // if you use STL, please include this line AFTER all other include
#include "Vertex.h"
#include "Shaders.h"
#include <conio.h>
#include "Globals.h"
#include "Camera.h"
#include "MouseEnums.h"
#include <iostream>

#define PI 3.14

GLuint vboId;
GLuint lineVboId;
Shaders myShaders;
Shaders lineShaders;
Camera myCamera;

float angle, step = 0.001f;
float totalTime = 0.0f;

int Init(ESContext* esContext)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Vertex verticesData[6];
	verticesData[0].pos = Vector3(-0.5f, 0.5f, 0.0f); verticesData[0].color = Vector3(1.0f, 0.0f, 0.0f); // Roșu
	verticesData[1].pos = Vector3(-0.5f, -0.5f, 0.0f); verticesData[1].color = Vector3(0.0f, 1.0f, 0.0f); // Verde
	verticesData[2].pos = Vector3(0.5f, -0.5f, 0.0f); verticesData[2].color = Vector3(0.0f, 0.0f, 1.0f); // Albastru
	verticesData[3].pos = Vector3(-0.5f, 0.5f, 0.0f); verticesData[3].color = Vector3(1.0f, 0.0f, 0.0f); // Roșu
	verticesData[4].pos = Vector3(0.5f, 0.5f, 0.0f); verticesData[4].color = Vector3(1.0f, 1.0f, 0.0f); // Galben
	verticesData[5].pos = Vector3(0.5f, -0.5f, 0.0f); verticesData[5].color = Vector3(0.0f, 0.0f, 1.0f); // Albastru

	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData), verticesData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Vector3 lineVerticesData[2];
	lineVerticesData[0] = Vector3(0.0f, 1.0f, 0.0f);  
	lineVerticesData[1] = Vector3(0.0f, -1.0f, 0.0f);

	glGenBuffers(1, &lineVboId);
	glBindBuffer(GL_ARRAY_BUFFER, lineVboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerticesData), lineVerticesData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	lineShaders.Init("../Resources/Shaders/LineShaderVS.vs", "../Resources/Shaders/LineShaderFS.fs");
	return myShaders.Init("../Resources/Shaders/TriangleShaderVS.vs", "../Resources/Shaders/TriangleShaderFS.fs");
}

void Draw ( ESContext *esContext )
{

	Matrix mRotation;
	mRotation.SetRotationZ(angle);

	Matrix MVP;
	MVP.SetIdentity();
	MVP = mRotation * myCamera.viewMatrix * myCamera.perspectiveMatrix;



	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(myShaders.program);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	
	if(myShaders.positionAttribute != -1)
	{
		glEnableVertexAttribArray(myShaders.positionAttribute);
		glVertexAttribPointer(myShaders.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}

	if (myShaders.colorAttribute != -1)
	{
		glEnableVertexAttribArray(myShaders.colorAttribute);
		glVertexAttribPointer(myShaders.colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3)));
	}
	if (myShaders.matrixUniform != -1) {
		glUniformMatrix4fv(myShaders.matrixUniform, 1, GL_FALSE, (float*)MVP.m);
		//MVP.print();
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);


	glUseProgram(lineShaders.program);
	
	glBindBuffer(GL_ARRAY_BUFFER, lineVboId);

	if (lineShaders.positionAttribute != -1)
	{
		glEnableVertexAttribArray(lineShaders.positionAttribute);
		glVertexAttribPointer(lineShaders.positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
	}

	glDrawArrays(GL_LINES, 0, 2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
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

