#pragma once

#include <string>

class Shader
{
public:
	Shader(const uint32_t type, const std::string source);
	~Shader();

	const uint32_t getID() const { return shaderID; }

private:
	uint32_t shaderID;
};

