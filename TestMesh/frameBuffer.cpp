#include "frameBuffer.h"
#include <iostream>

framebuffer::framebuffer(bool isDefaultFramebuffer, bool useTextureArrays) :
    _id(0),
    _maxColorAttachments(0),
    _currentAttachment(0),
    _drawBuffers(std::vector<GLenum>()),
	_useTextureArrays(useTextureArrays)
{
    if (!isDefaultFramebuffer)
    {
        glCreateFramebuffers(1, &_id);
    }

    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_maxColorAttachments);
}

framebuffer::~framebuffer()
{
    glDeleteFramebuffers(1, &_id);
}

void framebuffer::add(renderTarget* renderTarget)
{
    if (_id == 0)
    {
        std::cout << "trying to add render target to default framebuffer!?" << std::endl;
        return;
    }

    auto att = renderTarget->attachment;

    if (!(att == GL_DEPTH_ATTACHMENT ||
        att == GL_STENCIL_ATTACHMENT ||
        att == GL_DEPTH_STENCIL_ATTACHMENT))
        _drawBuffers.push_back(att);

    glBindFramebuffer(GL_FRAMEBUFFER, _id);
	if (_useTextureArrays)
	{
		glNamedFramebufferTextureLayer(
			_id,
			att,
			renderTarget->address.containerId,
			0,
			static_cast<GLint>(renderTarget->address.page));
	}
	else
	{
		glNamedFramebufferTexture(
			_id,
			att,
			renderTarget->tex->id,
			0
		);
	}
    auto status = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);
    
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "complete" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "incomplete attachment" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cout << "incomplete draw buffer" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        std::cout << "incomplete layer targets" << std::endl;
        break;
    default:
        break;
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::bind(GLenum target)
{
    glBindFramebuffer(target, _id);
}

void framebuffer::bindForDrawing()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);

	if (_drawBuffers.size() > 0)
		glDrawBuffers((GLsizei)_drawBuffers.size(), &_drawBuffers[0]);
}

void framebuffer::bindForDrawing(GLenum * buffers, GLsizei buffersCount)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);

    glDrawBuffers(buffersCount, buffers);
}

void framebuffer::bindForReading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void framebuffer::bindForReading(renderTarget * sourceRenderTarget)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);

    glReadBuffer(sourceRenderTarget->attachment);
}

void framebuffer::unbind(GLenum target)
{
    glBindFramebuffer(target, 0);
}

void framebuffer::blitToDefault(renderTarget * renderTarget, int x, int y, int w, int h)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    bindForReading(renderTarget);

	if (w == -1)
		w = renderTarget->w;

	if (h == -1)
		h = renderTarget->h;

    glBlitFramebuffer(0, 0, renderTarget->w, renderTarget->h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void framebuffer::blit(framebuffer * sourceFramebuffer, renderTarget * sourceRenderTarget, framebuffer * targetFramebuffer, renderTarget * targetRenderTarget)
{
    sourceFramebuffer->bindForReading(sourceRenderTarget);

    glBlitFramebuffer(
        0,
        0,
        sourceRenderTarget->w,
        sourceRenderTarget->h,
        0,
        0,
        targetRenderTarget->w,
        targetRenderTarget->h,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR);
}

GLfloat framebuffer::getZBufferValue(int x, int y)
{
    bindForReading();
    GLfloat zBufferValue;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);

    return zBufferValue;
}

