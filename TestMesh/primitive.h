#pragma once

#include "vertexBufferObject.h"
#include "elementBufferObject.h"
#include "parameterSemantic.h"

#include <GL\glew.h>

#include <map>

class primitive
{
private:
    GLenum _mode;
    std::unordered_map<parameterSemantic, vertexBufferObject*> _vbos;
    elementBufferObject* _ebo;

public:
    primitive(GLenum mode, std::unordered_map<parameterSemantic, vertexBufferObject*> vbos, elementBufferObject* ebo);
    ~primitive();

    void render();

    vertexBufferObject* findVbo(parameterSemantic semantic);
    elementBufferObject * getEbo();
};
