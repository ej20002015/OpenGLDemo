#include "DepthMap.h"

#include "GL/glew.h"

#include <iostream>

DepthMap::DepthMap(uint32_t width, uint32_t height)
	: width(width), height(height)
{
	invalidate();
}

DepthMap::~DepthMap()
{
	glDeleteTextures(1, &depthAttachmentID);
	glDeleteFramebuffers(1, &ID);
}

void DepthMap::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(0, 0, width, height);
}

void DepthMap::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthMap::bindDepthAttachment(uint32_t slot)
{
	glBindTextureUnit(slot, depthAttachmentID);
}

void DepthMap::resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0 || width > MAX_DEPTHMAP_SIZE || height > MAX_DEPTHMAP_SIZE)
	{
		std::cerr << "ERROR: Framebuffer resize values are invalid" << std::endl;
		exit(1);
	}

	height = height;
	width = width;
}

void DepthMap::invalidate()
{
	//Delete previous buffer and depth attachment if initialised
	if (ID)
	{
		glDeleteTextures(1, &depthAttachmentID);
		glDeleteFramebuffers(1, &ID);
	}

	//Create framebuffer for depth map
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	//Generate depth texture
	glGenTextures(1, &depthAttachmentID);
	glBindTexture(GL_TEXTURE_2D, depthAttachmentID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Attach to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentID, 0);
	//Tell opengl we are not going to render any colour data
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
		std::cerr << "ERROR: Framebuffer is not complete" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}