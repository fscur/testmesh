#pragma once
#include "textureAddress.h"
#include "texture.h"

class renderTarget
{
public:
    GLenum attachment;
    GLint w;
    GLint h;
    textureAddress address;
    texture* tex;
public:
    renderTarget(
        GLenum attachment,
        GLint w, 
        GLint h,
        textureAddress address,
        texture* tex) :
        attachment(attachment),
        w(w),
        h(h),
        address(address),
        tex(tex)
    {
    }

    ~renderTarget()
    {
        delete tex;
    }
};
