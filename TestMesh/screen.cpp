#include "screen.h"
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void screen::initFont()
{
    _font = new font("Consola.ttf", 15);
    std::wstring text = L"auto kern = _font->getKerning(previousGlyph, glyph);";

    //std::wstring text = L"Filipe Scur é um cara #%5267!@#$%¨&*(QwErTYUIO~dâ^dá´r";

    for (int i = 0; i < text.length(); i++)
        _font->getGlyph((ulong)text[i]);
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
    _modelMatrix = glm::mat4(1.0f);
}

void screen::initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");

    _shader->addAttribute("inPosition");
    _shader->addAttribute("inTexCoord");
    _shader->addAttribute("inNormal");

    _shader->init();
    _shader->addUniform("m", 0);
    _shader->addUniform("v", 1);
    _shader->addUniform("p", 2);
    _shader->addUniform("glyphTexture", 3);
    _shader->addUniform("texPos", 4);
    _shader->addUniform("texSize", 5);
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

void screen::onInit()
{
    initGL();
    initFont();
    createQuad();
    initShader();
    initCamera();
}

void screen::onUpdate()
{
}

void screen::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shader->bind();
    _shader->getUniform(1).set(_viewMatrix);
    _shader->getUniform(2).set(_projectionMatrix);
    _shader->getUniform(3).set(_font->getGlyphAtlasId(), 0);

    std::wstring text = L"auto kern = _font->getKerning(previousGlyph, glyph);";
    auto atlasSize = (float)_font->getGlyphAtlasSize();
    //std::wstring text = L"Filipe Scur é um cara #%5267!@#$%¨&*(QwErTYUIO~dâ^dá´r";

    float x = 0.0f;
    float lineHeight = (float)_font->getLineHeight();
    float y = lineHeight;

    glyph* previousGlyph = nullptr;

    for (int i = 0; i < text.length(); i++)
    {
        auto glyph = _font->getGlyph((ulong)text[i]);
        auto kern = _font->getKerning(previousGlyph, glyph);
        auto w = (float)glyph->width;
        auto h = (float)glyph->height;

        _modelMatrix = glm::mat4(
          w, 0.0f, 0.0f, 0.0f,
          0.0f, h, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          x + glyph->horiBearingX, -y -h + glyph->horiBearingY, 0.0f, 1.0f);

        x += glyph->horiAdvance + kern.x;
        
        _shader->getUniform(0).set(_modelMatrix);
        _shader->getUniform(4).set(glyph->texPos);
        _shader->getUniform(5).set(glyph->texSize);

        _quad->render();

        previousGlyph = glyph;
    }

    _shader->unbind();
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
    delete _font;
}