#include "Texture.h"

#include "GL/glew.h"

#include "stb_image/stb_image.h"

Texture::Texture(const std::string& filePath)
	: ID(0), filePath(filePath), localBuffer(nullptr), width(0), height(0), bytesPerPixel(0)
{
	//openGL expects pixels to start at bottom left (.png starts at the top) so y direction needs to be flipped
	stbi_set_flip_vertically_on_load(1);

	localBuffer = stbi_load(filePath.c_str(), &width, &height, &bytesPerPixel, 4);

	glCreateTextures(GL_TEXTURE_2D, 1, &ID);
	glTextureStorage2D(ID, 1, GL_RGBA8, width, height);

	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
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
