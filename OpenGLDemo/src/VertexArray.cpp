#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &ID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &ID);
}

void VertexArray::addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& vertexBufferLayout)
{
	bind();
	vertexBuffer.bind();
	const auto& attributes = vertexBufferLayout.getAttributes();
	uint32_t offset = 0;
	for (uint32_t i = 0; i < attributes.size(); ++i)
	{
		const auto& attribute = attributes.at(i);
		glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalised,
			vertexBufferLayout.getStride(), (const void*)offset);
		glEnableVertexAttribArray(i);
		offset += attribute.count * VertexBufferLayoutAttribute::getSizeOfType(attribute.type);
	}
}

void VertexArray::bind() const
{
	glBindVertexArray(ID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}
