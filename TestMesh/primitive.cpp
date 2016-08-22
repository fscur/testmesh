#include "primitive.h"

primitive::primitive(GLenum mode, std::unordered_map<parameterSemantic, vertexBufferObject*> vbos, elementBufferObject* ebo) :
    _mode(mode),
    _vbos(vbos),
    _ebo(ebo)
{
}

primitive::~primitive()
{
}

void primitive::render()
{
    glDrawElements(_mode, _ebo->getComponentCount(), _ebo->getComponentType(), 0);
}

vertexBufferObject* primitive::findVbo(parameterSemantic semantic)
{
    return _vbos[semantic];
}

elementBufferObject* primitive::getEbo()
{
    return _ebo;
}