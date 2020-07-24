#include "FrameBuffer.h"

#include <iostream>

Framebuffer::Framebuffer(const FramebufferSpecification& specification)
	: specification(specification)
{
	invalidate();
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &ID);
	glDeleteTextures(1, &colourAttachmentID);
	glDeleteTextures(1, &depthAttachmentID);
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(0, 0, specification.width, specification.height);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindColourAttachment(uint32_t slot)
{
	glBindTextureUnit(slot, colourAttachmentID);
}

void Framebuffer::resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
	{
		std::cerr << "ERROR: Framebuffer resize values are invalid" << std::endl;
		exit(1);
	}

	specification.height = height;
	specification.width = width;
}

void Framebuffer::blit(const Framebuffer& otherFramebuffer)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, otherFramebuffer.ID);
	glBlitFramebuffer(0, 0, specification.width, specification.height, 0, 0, otherFramebuffer.specification.width, otherFramebuffer.specification.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Framebuffer::invalidate()
{
	//Delete previous buffer and attachments if initialised
	if (ID)
	{
		glDeleteFramebuffers(1, &ID);
		glDeleteTextures(1, &colourAttachmentID);
		glDeleteTextures(1, &depthAttachmentID);
	}

	//Create framebuffer
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glGenTextures(1, &colourAttachmentID);
	glGenTextures(1, &depthAttachmentID);
	//If samples is greater than one then create multisample texture attachments
	if (specification.samples > 1)
	{
		//Colour attachment
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colourAttachmentID);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.samples, GL_RGBA8, specification.width, specification.height, true);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colourAttachmentID, 0);

		//Depth and stencil attachment
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthAttachmentID);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.samples, GL_DEPTH24_STENCIL8, specification.width, specification.height, true);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthAttachmentID, 0);
	}
	else
	{
		//Colour attachment
		glBindTexture(GL_TEXTURE_2D, colourAttachmentID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification.width, specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourAttachmentID, 0);

		//Depth and stencil attachment
		glBindTexture(GL_TEXTURE_2D, depthAttachmentID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification.width, specification.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentID, 0);
	}

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
		std::cerr << "ERROR: Framebuffer is not complete" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
