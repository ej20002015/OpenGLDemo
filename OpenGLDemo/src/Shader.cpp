#include "Shader.h"

#include "GL/glew.h"
#include <iostream>

Shader::Shader(const uint32_t type, const std::string source)
{
	shaderID = glCreateShader(type);
	const char* cSource = source.c_str();
	glShaderSource(shaderID, 1, &cSource, nullptr);
	glCompileShader(shaderID);

	//Check compilation status
	int32_t compilationStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus);
	if (!compilationStatus)
	{
		int32_t logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* logMessage = (char*)alloca(logLength * sizeof(char));
		glGetShaderInfoLog(shaderID, logLength, &logLength, logMessage);
		std::cerr << "ERROR: Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader - " << logMessage << std::endl;
	}
		
}

Shader::~Shader()
{
	glDeleteShader(shaderID);
}
