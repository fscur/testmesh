#include "vertexBuffer.h"

vertexBuffer::vertexBuffer(std::vector<vertexAttrib> attribs) :
    buffer(bufferTarget::array)
{
    auto s = attribs.size();

    for (GLuint i = 0; i < s; i++)
    {
        auto location = attribs[i].location;

        glEnableVertexAttribArray(location);

        if (attribs[i].type == GL_UNSIGNED_INT || attribs[i].type == GL_INT)
        {
            glVertexAttribIPointer(
                location, 
                attribs[i].size, 
                attribs[i].type, 
                attribs[i].stride, 
                attribs[i].offset);
        }
        else if (attribs[i].type == GL_FLOAT)
        {
            glVertexAttribPointer(
                location, 
                attribs[i].size, 
                attribs[i].type, 
                GL_FALSE, 
                attribs[i].stride, 
                attribs[i].offset);
        }

        glVertexAttribDivisor(location, attribs[i].divisor);
    }
}
