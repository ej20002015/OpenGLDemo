#pragma once

#include <vector>
#include <iostream>

#include "GL/glew.h"

struct VertexBufferLayoutAttribute
{
	uint32_t type;
	uint32_t count;
	unsigned char normalised;

	static uint32_t getSizeOfType(uint32_t type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return sizeof(GLfloat);
			break;

		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
			break;

		case GL_UNSIGNED_BYTE:
			return sizeof(GLubyte);
			break;

		default:
			std::cerr << "ERROR: Type not supported" << std::endl;
			exit(1);
			break;
		}
	}
};

class VertexBufferLayout
{
public:
	VertexBufferLayout()
		: stride(0) {}

	template<typename T>
	void push(uint32_t count) { static_assert(false); }
	
	template<>
	void push<float>(uint32_t count) 
	{
		attributes.push_back({GL_FLOAT, count, GL_FALSE});
		stride += count * VertexBufferLayoutAttribute::getSizeOfType(GL_FLOAT);
	}

	template<>
	void push<uint32_t>(uint32_t count)
	{ 
		attributes.push_back({GL_UNSIGNED_INT, count, GL_FALSE}); 
		stride += count * VertexBufferLayoutAttribute::getSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void push<unsigned char>(uint32_t count) 
	{
		attributes.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
		stride += count * VertexBufferLayoutAttribute::getSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferLayoutAttribute>& getAttributes() const { return attributes; }
	inline const uint32_t getStride() const { return stride; }

private:
	std::vector<VertexBufferLayoutAttribute> attributes;
	uint32_t stride;
};