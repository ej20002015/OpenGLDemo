#include "Program.h"

#include "GL/glew.h"

#include <fstream>
#include <sstream>
#include <iostream>

Program::Program(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	: programID(0), vertexShaderID(0), fragmentShaderID(0), filePath("None")
{
	createProgram(vertexShaderSource, fragmentShaderSource);
}

Program::Program(const std::string& filePath)
	: programID(0), vertexShaderID(0), fragmentShaderID(0), filePath(filePath)
{

	enum class ShaderSourceType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::ifstream inputStream(filePath);
	std::string line;
	std::stringstream shaderSource[2];
	ShaderSourceType sourceType = ShaderSourceType::NONE;

	while (getline(inputStream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				sourceType = ShaderSourceType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				sourceType = ShaderSourceType::FRAGMENT;
		}
		else
		{
			shaderSource[(int)sourceType] << line << "\n";
		}
	}

	createProgram(shaderSource[0].str(), shaderSource[1].str());
}

Program::~Program()
{
	glDeleteProgram(programID);
}

void Program::setUniform1i(const std::string& name, int32_t i0)
{
	glUniform1i(getUniformLocation(name), i0);
}

void Program::setUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	glUniform4f(getUniformLocation(name), f0, f1, f2, f3);
}

void Program::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Program::bind() const
{
	glUseProgram(programID);
}

void Program::unbind() const
{
	glUseProgram(0);
}

void Program::createProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	programID = glCreateProgram();

	Shader vertexShader(GL_VERTEX_SHADER, vertexShaderSource);
	Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	vertexShaderID = vertexShader.getID();
	fragmentShaderID = fragmentShader.getID();
	
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	glValidateProgram(programID);

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
}

int32_t Program::getUniformLocation(const std::string& name)
{
	if (uniformLocationCache.find(name) != uniformLocationCache.end())
		return uniformLocationCache.at(name);


	int32_t location = glGetUniformLocation(programID, name.c_str());
	if (location == -1)
		std::cout << "WARNING: Uniform not found - " << name << std::endl;

	uniformLocationCache[name] = location;
	return location;
}
