#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Program.h"

#include <vector>
#include <memory>

class Mesh
{
public:
	Mesh(const std::shared_ptr<VertexArray> vertexArray, const std::shared_ptr<IndexBuffer> indexBuffer, const std::vector<std::shared_ptr<Texture>>& textures)
		: vertexArray(vertexArray), indexBuffer(indexBuffer), textures(textures) {}
	
	inline const std::shared_ptr<VertexArray> getVertexArray() const { return vertexArray; }
	inline const std::shared_ptr<IndexBuffer> getIndexBuffer() const { return indexBuffer; }
	inline const std::vector<std::shared_ptr<Texture>>& getTextures() const { return textures; }
	
private:
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::vector<std::shared_ptr<Texture>> textures;
};