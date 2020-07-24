#include "Cubemap.h"

#include "stb_image/stb_image.h"

#include <iostream>

Cubemap::Cubemap(const std::array<std::string, 6> filePaths)
	: filePaths(filePaths)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	//Load all six textures of the cubemap
	for (uint32_t i = 0; i < 6; ++i)
	{
		localBuffers[i] = stbi_load(filePaths[i].c_str(), &widths[i], &heights[i], &bytesPerPixels[i], 0);

		if (stbi_failure_reason())
			std::cerr << "ERROR: Error occured when loading texture - " << stbi_failure_reason() << std::endl;

		GLenum internalFormat, format;

		switch (bytesPerPixels[i])
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
			if (localBuffers[i])
				stbi_image_free(localBuffers[i]);

			return;
			break;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, widths[i], heights[i], 0, format, GL_UNSIGNED_BYTE, localBuffers[i]);

		if (localBuffers[i])
			stbi_image_free(localBuffers[i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &ID);
}

void Cubemap::bind(uint32_t slot) const 
{
	glBindTextureUnit(slot, ID);
}

void Cubemap::unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
