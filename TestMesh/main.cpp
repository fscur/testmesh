#include "application.h"
#include "screen.h"

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char* args[])
{
    application app;

    auto mainScreen = new screen("TestMesh", 1600, 900);
    app.run(mainScreen);
    delete mainScreen;

    return 0;
}