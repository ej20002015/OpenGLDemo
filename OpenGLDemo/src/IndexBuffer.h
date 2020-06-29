#pragma once

#include <cstdint>

class IndexBuffer
{
public:
	IndexBuffer(const uint32_t* data, uint32_t count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	inline const uint32_t getCount() const { return count; }
	
private:
	uint32_t ID, count;
};