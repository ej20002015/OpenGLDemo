#pragma once

#include <string>

class Texture
{
public:
	Texture(const std::string& filePath);
	~Texture();

	void bind(uint32_t slot = 0) const;
	void unbind() const;

	inline uint32_t getWidth() const { return width; }
	inline uint32_t getHeight() const { return height; }

private:
	uint32_t ID;
	std::string filePath;
	unsigned char* localBuffer;
	int32_t width, height, bytesPerPixel;
};