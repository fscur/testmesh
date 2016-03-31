#pragma once

#include "window.h"

#include <string>
#include "window.h"

class application
{
private:
    static bool _initialized;

public:
    static uint framesPerSecond;
    static double millisecondsPerFrame;

private:
    bool _running;
    window* _window;

private:
    void onInit();
    void onClear();
    void onRender();
    void onInput();
    void onUpdate();
    void onTick();
    void onSwapbuffers();
    void onClose();

public:
    application();
    ~application();

    void run(window* window);
    static void logError(std::string message);
    static void logWarning(std::string message);
    static void logInfo(std::string message);
};