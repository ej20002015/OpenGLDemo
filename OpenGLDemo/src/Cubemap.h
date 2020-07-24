#pragma once

#include "GL/glew.h"

#include <cstdint>
#include <string>
#include <array>

class Cubemap
{
public:
	Cubemap(const std::array<std::string, 6> filePaths);
	~Cubemap();

	void bind(uint32_t slot = 0) const;
	void unbind() const;

private:
	uint32_t ID = 0;
	std::array<std::string, 6> filePaths;
	std::array<int32_t, 6> widths = { 0 }, heights = { 0 }, bytesPerPixels = { 0 };
	std::array<unsigned char*, 6> localBuffers = { nullptr };
};