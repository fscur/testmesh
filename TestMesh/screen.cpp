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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void screen::initFont()
{
    _texturesManager = new texturesManager(false, false, 1);
    _fontManager = new fontManager(_texturesManager);
    _font = new font("Consola.ttf", 15);
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
    _shader->addUniform("glyphAtlas", 3);
    _shader->addUniform("quadPos", 4);
    _shader->addUniform("quadSize", 5);
    _shader->addUniform("texel", 6);
    _shader->addUniform("shift", 7);
    _shader->addUniform("glyphPage", 8);
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
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendColor(1, 1, 1, 1);

    _shader->bind();
    _shader->getUniform(1).set(_viewMatrix);
    _shader->getUniform(2).set(_projectionMatrix);
    _shader->getUniform(3).set(0);

    std::wstring text = L"Fuck Yeah!";
    auto atlasSize = (float)_font->getGlyphAtlasSize();
    //std::wstring text = L"Filipe Scur é um cara #%5267!@#$%¨&*(QwErTYUIO~dâ^dá´r";
    _shader->getUniform(6).set(glm::vec2(1.0f / atlasSize, 1.0f / atlasSize));

    float lineHeight = (float)_font->getLineHeight();

    for (int j = 0; j < 1; j++)
    {
        float x = j * 0.1f;
        float y = lineHeight * (j + 1);

        glyph* previousGlyph = nullptr;

        for (int i = 0; i < text.length(); i++)
        {
            auto glyph = _fontManager->getGlyph(_font, (ulong)text[i]);
            auto kern = _font->getKerning(previousGlyph, glyph);
            auto w = (float)glyph->width;
            auto h = (float)glyph->height;
            auto x0 = x + glyph->offsetX;// +glyph->horiBearingX;

            _modelMatrix = glm::mat4(
                w, 0.0f, 0.0f, 0.0f,
                0.0f, h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x0, -y - h + glyph->offsetY, 0.0f, 1.0f);

            x += glyph->horiAdvance + kern.x;

            _shader->getUniform(0).set(_modelMatrix);
            _shader->getUniform(4).set(glyph->texPos);
            _shader->getUniform(5).set(glyph->texSize);

            float dx0 = std::abs(x0 - ((int)x0));
            _shader->getUniform(7).set(dx0);
            _shader->getUniform(8).set(0);

            _quad->render();

            previousGlyph = glyph;
        }
    }

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
    delete _font;
}