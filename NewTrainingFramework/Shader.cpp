#include "stdafx.h"
#include "Shader.h"

Shader::Shader(ShaderResource* resource)
    : sr(resource), programId(0) {}

Shader::~Shader() {
    glDeleteProgram(programId);
}

void Shader::Load() {

	GLuint vertexShader, fragmentShader;

	char* vertexFilePath = new char[sr->vertexShaderPath.size() + 1];
	std::copy(sr->vertexShaderPath.begin(), sr->vertexShaderPath.end(), vertexFilePath);
	vertexFilePath[sr->vertexShaderPath.size()] = '\0';

	char* fragmentFilePath = new char[sr->fragmentShaderPath.size() + 1];
	std::copy(sr->fragmentShaderPath.begin(), sr->fragmentShaderPath.end(), fragmentFilePath);
	fragmentFilePath[sr->fragmentShaderPath.size()] = '\0';

	vertexShader = esLoadShader(GL_VERTEX_SHADER, vertexFilePath);
	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fragmentFilePath);

	programId = esLoadProgram(vertexShader, fragmentShader);

	if (!programId) {
		std::cerr << "Failed to load shader: " << sr->vertexShaderPath << " and " << sr->fragmentShaderPath << std::endl;
	}
	else {
		std::cout << "Shader program loaded successfully" << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	positionAttribute = glGetAttribLocation(programId, "a_posL");
	colorAttribute = glGetAttribLocation(programId, "a_color");
	matrixUniform = glGetUniformLocation(programId, "u_MVP");
	uvAttribute = glGetAttribLocation(programId, "a_uv");
	textureUniform = glGetUniformLocation(programId, "u_texture");

}

