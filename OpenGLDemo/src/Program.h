#pragma once

#include <string>
#include <unordered_map>

#include "Shader.h"
#include "glm/glm.hpp"

class Program
{
public:
	Program(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	Program(const std::string& filePath);
	~Program();

	void setUniform1i(const std::string& name, int32_t i0);
	void setUniform4f(const std::string& name, float f0, float f1, float f2, float f3);

	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

	inline const uint32_t getID() const { return programID; }
	void bind() const;
	void unbind() const;

private:
	void createProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	int32_t getUniformLocation(const std::string& name);
	uint32_t programID, vertexShaderID, fragmentShaderID;
	std::string filePath;
	std::unordered_map<std::string, int32_t> uniformLocationCache;
};