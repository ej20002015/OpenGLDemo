#pragma once

#include "GL/glew.h"

#include <cstdint>

class Framebuffer
{
public:

	static const uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

	struct FramebufferSpecification
	{
		uint32_t width, height;
		uint32_t samples = 1;
	};

	Framebuffer(const FramebufferSpecification& specification);
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;

	void bind();
	void unbind();

	void bindColourAttachment(uint32_t slot = 0);

	void resize(uint32_t width, uint32_t height);
	void blit(const Framebuffer& otherFramebuffer);
	
	inline uint32_t getColourAttachmentID() const { return colourAttachmentID; }

private:
	void invalidate();

private:
	uint32_t ID = 0, colourAttachmentID = 0, depthAttachmentID = 0;
	FramebufferSpecification specification;
};