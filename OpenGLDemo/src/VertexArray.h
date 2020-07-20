#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray& vertexArray) = delete;

	void addBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer, const VertexBufferLayout& vertexBufferLayout);

	void bind() const;
	void unbind() const;
private:
	uint32_t ID;
	std::shared_ptr<VertexBuffer> vertexBuffer;
};