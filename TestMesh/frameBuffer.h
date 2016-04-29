#pragma once
#include "renderTarget.h"

class framebuffer
{
private:
    GLuint _id;
    GLint _maxColorAttachments;
    GLint _currentAttachment;
    std::vector<GLenum> _drawBuffers;
	bool _useTextureArrays;

public:
    framebuffer(bool isDefaultFramebuffer = false, bool useTextureArrays = false);
    ~framebuffer();

    void add(renderTarget* renderTarget);
    void bind(GLenum target);
    void bindForDrawing();
    void bindForDrawing(GLenum* buffers, GLsizei buffersCount);
    void bindForReading();
    void bindForReading(renderTarget* sourceRenderTarget);
    void unbind(GLenum target);
    void blitToDefault(renderTarget* renderTarget, int x = 0, int y = 0, int w = -1, int h = -1);
    void blit(
        framebuffer* sourceFramebuffer,
        renderTarget* sourceRenderTarget,
        framebuffer* targetFramebuffer,
        renderTarget* targetRenderTarget);

    GLfloat getZBufferValue(int x, int y);
};
