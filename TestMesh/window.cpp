#include "window.h"

#include "input.h"

window::window(std::string name, uint width, uint height) :
    _name(name),
    _width(width),
    _height(height),
    closed(false)
{
    input::mouseDown->assign(std::bind(&window::onMouseDown, this, std::placeholders::_1));
    input::mouseUp->assign(std::bind(&window::onMouseUp, this, std::placeholders::_1));
    input::mouseMove->assign(std::bind(&window::onMouseMove, this, std::placeholders::_1));
    input::mouseWheel->assign(std::bind(&window::onMouseWheel, this, std::placeholders::_1));
}

window::~window()
{
}

void window::clear()
{
}

void window::render()
{
    onRender();
}

void window::update()
{
    onUpdate();
}

void window::tick()
{
    onTick();
}

void window::onMouseDown(mouseEventArgs* eventArgs)
{
}

void window::onMouseUp(mouseEventArgs* eventArgs)
{
}

void window::onMouseMove(mouseEventArgs* eventArgs)
{
}

void window::onMouseWheel(mouseEventArgs* eventArgs)
{
}

void window::onTick() {}