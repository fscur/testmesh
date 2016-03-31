#pragma once
#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"

class window
{
protected:
    std::string _name;
    uint _width;
    uint _height;

public:
    bool closed;

private:
    void adjustWindowToScreenBounds();
public:
    window(std::string name, uint width, uint height);
    virtual ~window();

    void init();
    void clear();
    void render();
    void input();
    void update();
    void tick();
    void swapBuffers();
    void close();

    virtual void onInit() = 0;
    virtual void onUpdate() = 0;
    virtual void onRender() = 0;
    virtual void onClosing() = 0;

    virtual void onTick();
};