#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& vertexBufferLayout);

	void bind() const;
	void unbind() const;
private:
	uint32_t ID;
};