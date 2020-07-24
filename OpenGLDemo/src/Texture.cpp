#include "Texture.h"

#include "GL/glew.h"
#include "stb_image/stb_image.h"

#include <iostream>

Texture::Texture(const std::string& filePath, Type type)
	: ID(0), filePath(filePath), type(type), localBuffer(nullptr), width(0), height(0), bytesPerPixel(0)
{
	//openGL expects pixels to start at bottom left so y direction needs to be flipped
	stbi_set_flip_vertically_on_load(1);

	localBuffer = stbi_load(filePath.c_str(), &width, &height, &bytesPerPixel, 0);

	if (stbi_failure_reason())
		std::cerr << "ERROR: Error occured when loading texture - " << stbi_failure_reason() << std::endl;

	GLenum internalFormat, format;

	switch (bytesPerPixel)
	{
	case 1:
		internalFormat = GL_R8;
		format = GL_RED;
		break;
	case 3:
		internalFormat = GL_RGB8;
		format = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
		break;
	default:
		std::cerr << "ERROR: Cannot load texture format" << std::endl;
		if (localBuffer)
			stbi_image_free(localBuffer);

		return;
		break;
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &ID);
	glTextureStorage2D(ID, 1, internalFormat, width, height);

	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureSubImage2D(ID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, localBuffer);
	glGenerateTextureMipmap(ID);

	//Free cpu memory now its on the GPU
	if (localBuffer)
		stbi_image_free(localBuffer);

}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::bind(uint32_t slot) const
{
	glBindTextureUnit(slot, ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
