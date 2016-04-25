#include "screen.h"
#include "application.h"

#include <iostream>

screen::screen(std::string name, uint width, uint height) :
    window(name, width, height)
{
}

screen::~screen()
{
}

void screen::onInit()
{
}

void screen::onUpdate()
{
}

void screen::onRender()
{  
}

void screen::onTick()
{
}

void screen::onClosing()
{
}