#pragma once

#include <cstdint>

class DepthMap
{
public:

	static const uint32_t MAX_DEPTHMAP_SIZE = 8192;

	DepthMap(uint32_t width, uint32_t height);
	~DepthMap();

	DepthMap(const DepthMap&) = delete;

	void bind();
	void unbind();

	void bindDepthAttachment(uint32_t slot = 0);

	void resize(uint32_t width, uint32_t height);

private:
	void invalidate();

private:
	uint32_t ID = 0, depthAttachmentID = 0;
	uint32_t width, height;
};