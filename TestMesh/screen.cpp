#include "screen.h"
#include "texture.h"
#include "application.h"
#include <gl\glew.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>

screen::screen(std::string name, uint width, uint height) :
    window(name, width, height)
{
}

screen::~screen()
{
}

void screen::initGL()
{
    //glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void screen::initFont()
{
    _texturesManager = new texturesManager(false, false, 1);
    _fontManager = new fontManager(_texturesManager);
    _font0 = new font("Consola.ttf", 24);
    _font1 = new font("SegoeUI-Light.ttf", 24);
    _font2 = new font("Roboto-Thin.ttf", 24);
    _font3 = new font("Vera.ttf", 24);
}

void screen::createVao()
{
    glCreateVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    auto geometry = _quad;
    createVbo(geometry->vboData, geometry->vboSize);
    createEbo(geometry->eboData, geometry->eboSize);
    createGlyphIdsBuffer(nullptr, sizeof(glyphInfo));
    createModelMatricesBuffer(nullptr, sizeof(glm::mat4));

    glBindVertexArray(0);
}

void screen::createVbo(void* data, GLsizeiptr size)
{
    std::vector<vertexAttrib> attribs;
    attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
    attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
    attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
    attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

    _vbo = new vertexBuffer(attribs);
    _vbo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
}

void screen::createGlyphIdsBuffer(void* data, GLsizeiptr size)
{
    std::vector<vertexAttrib> attribs;
    attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));

    _glyphIdsBuffer = new vertexBuffer(attribs);
    _glyphIdsBuffer->data(size, data, bufferDataUsage::dynamicDraw);
}

void screen::createModelMatricesBuffer(void* data, GLsizeiptr size)
{
    std::vector<vertexAttrib> attribs;

    for (uint i = 0; i < 4; ++i)
        attribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(glm::mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

    _modelMatricesBuffer = new vertexBuffer(attribs);
    _modelMatricesBuffer->data(size, data, bufferDataUsage::dynamicDraw);
}

void screen::createEbo(void* data, GLsizeiptr size)
{
    _ebo = new buffer(bufferTarget::element);
    _ebo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
}

void screen::createQuad()
{
    auto vertices = std::vector<vertex>
    {
        vertex(glm::vec3(0.0f, 0.0f, +0.0f), glm::vec2(0.0f, 1.0f)),
        vertex(glm::vec3(1.0f, 0.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
        vertex(glm::vec3(1.0f, 1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
        vertex(glm::vec3(0.0f, 1.0f, +0.0f), glm::vec2(0.0f, 0.0f))
    };

    auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

    _quad = geometry::create(vertices, indices);
}

void screen::initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");

    _shader->addAttribute("inPosition");
    _shader->addAttribute("inTexCoord");
    _shader->addAttribute("inNormal");
    _shader->addAttribute("inTangent");
    _shader->addAttribute("inGlyphId");
    _shader->addAttribute("inModelMatrix");

    _shader->init();
    _shader->addUniform("v", 0);
    _shader->addUniform("p", 1);
    _shader->addUniform("glyphAtlas", 2);
    _shader->addUniform("texel", 3);
}

void screen::initCamera()
{
    auto size = 1.0f;
    auto hw = (float)_width * 0.5f;
    auto hh = (float)_height * 0.5f;

    _projectionMatrix = glm::ortho<float>(-hw, hw, -hh, hh, 0, 1000);

    _viewMatrix = glm::lookAt<float>(
        glm::vec3(hw, -hh, 1.0f),
        glm::vec3(hw, -hh, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void screen::initText()
{
    addText(L"The quick brown fox jumps over the lazy dog.", glm::vec2(10, 10), _font0);
    addText(L"The quick brown fox jumps over the lazy dog.", glm::vec2(10, 40), _font1);
    addText(L"The quick brown fox jumps over the lazy dog.", glm::vec2(10, 80), _font2);
    addText(L"The quick brown fox jumps over the lazy dog.", glm::vec2(10, 120), _font3);

    size_t glyphCount = _glyphInfos.size();
    std::vector<uint> glyphIds;
    for (size_t i = 0; i < glyphCount; i++)
        glyphIds.push_back(i);

    _glyphIdsBuffer->data(sizeof(uint) * glyphCount, &glyphIds[0], bufferDataUsage::dynamicDraw);

    _modelMatricesBuffer->data(sizeof(glm::mat4) * _modelMatrices.size(), &_modelMatrices[0], bufferDataUsage::dynamicDraw);

    _glyphInfoBuffer = new buffer(bufferTarget::uniform);
    _glyphInfoBuffer->data(sizeof(glyphInfo) * glyphCount, &_glyphInfos[0], bufferDataUsage::dynamicDraw);
    _glyphInfoBuffer->bindBufferBase(0);
}

void screen::onInit()
{
    initGL();
    initFont();
    createQuad();
    initShader();
    initCamera();
    createVao();
    initText();
}

void screen::onUpdate()
{
}

void screen::addText(std::wstring text, glm::vec2 position, font* font)
{
    float lineHeight = (float)font->getLineHeight();

    float x = position.x;
    float y = position.y + lineHeight;

    glyph* previousGlyph = nullptr;
    size_t textLength = text.length();

    for (size_t i = 0; i < textLength; i++)
    {
        auto glyph = _fontManager->getGlyph(font, (ulong)text[i]);
        auto kern = font->getKerning(previousGlyph, glyph);
        auto w = (float)glyph->width;
        auto h = (float)glyph->height;
        auto x0 = x + glyph->offsetX;

        _modelMatrix = glm::mat4(
            w, 0.0f, 0.0f, 0.0f,
            0.0f, h, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x0, -y - h + glyph->offsetY, 0.0f, 1.0f);

        _modelMatrices.push_back(_modelMatrix);

        x += glyph->horiAdvance + kern.x;
        float shift = std::abs(x0 - ((int)x0));

        glyphInfo info;
        info.pos = glyph->texPos;
        info.size = glyph->texSize;
        info.shift = shift;

        _glyphInfos.push_back(info);
        
        previousGlyph = glyph;
    }
}

void screen::onRender()
{
    auto texelSize = 1.0f / (float)_fontManager->getGlyphAtlasSize();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendColor(1, 1, 1, 1);

    _shader->bind();
    _shader->getUniform(0).set(_viewMatrix);
    _shader->getUniform(1).set(_projectionMatrix);
    _shader->getUniform(2).set(0);
    _shader->getUniform(3).set(glm::vec2(texelSize, texelSize));

    glBindVertexArray(_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _glyphInfos.size());
    glBindVertexArray(0);

    _shader->unbind();

    glBlendColor(0, 0, 0, 0);
    glDisable(GL_BLEND);
}

void screen::onTick()
{
    std::cout << "fps: " << application::framesPerSecond << std::endl;
}

void screen::onClosing()
{
    delete _quad;
    delete _shader;
    delete _camera;
    delete _font0;
    delete _font1;
    delete _fontManager;
    delete _texturesManager;
}