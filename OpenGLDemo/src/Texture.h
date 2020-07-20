#pragma once

#include <string>

class Texture
{
public:

	enum class Type
	{
		DIFFUSE,
		SPECULAR,
		OTHER
	};


	Texture(const std::string& filePath, Type type);
	~Texture();

	Texture(const Texture& texture) = delete;

	void bind(uint32_t slot = 0) const;
	void unbind() const;

	inline uint32_t getWidth() const { return width; }
	inline uint32_t getHeight() const { return height; }
	inline Type getType() const { return type; }
	inline const std::string& getFilePath() const { return filePath; }

private:
	uint32_t ID;
	std::string filePath;
	Type type;
	unsigned char* localBuffer;
	int32_t width, height, bytesPerPixel;
};