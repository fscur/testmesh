#pragma once
#include "types.h"
#include "window.h"
#include "fontManager.h"
#include "font.h"
#include "geometry.h"
#include "shader.h"
#include "camera.h"
#include "vertexBuffer.h"
#include <glm\glm.hpp>

class screen :
    public window
{
private:
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    camera* _camera;

    texturesManager* _texturesManager;
    fontManager* _fontManager;
    font* _font0;
    font* _font1;
    font* _font2;
    font* _font3;

    std::map<uint, GLuint> _glyphTextures;
    float _aspect = (float)_width / (float)_height;

    shader* _shader;
    geometry* _quad;

    GLuint _vao;
    vertexBuffer* _vbo;
    vertexBuffer* _modelMatricesBuffer;
    vertexBuffer* _glyphIdsBuffer;
    buffer* _ebo;

    std::vector<glm::mat4> _modelMatrices;
    buffer* _glyphInfoBuffer;
    std::vector<glyphInfo> _glyphInfos;

private:
    void initGL();
    void initFont();
    void createQuad();
    void initShader();
    void initCamera();
    void createVao();
    void createVbo(void* data, GLsizeiptr size);
    void createEbo(void* data, GLsizeiptr size); 
    void createGlyphIdsBuffer(void* data, GLsizeiptr size);
    void createModelMatricesBuffer(void* data, GLsizeiptr size);
    void initText();
    void addText(std::wstring text, glm::vec2 position, font* font);

public:
    screen(std::string name, uint witdh, uint height);
    ~screen(void);

    void onInit() override;
    void onClosing() override;
    void onUpdate() override;
    void onRender() override;
    void onTick() override;
};