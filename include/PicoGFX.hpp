#pragma once

#include "HardwareSPI.hpp"
#include "Display.hpp"
#include "Graphics.hpp"
#include "Print.hpp"
#include "AdvancedGraphics.hpp"

class PicoGFX {
public:
    PicoGFX(Display* display, Print* print, Graphics* graphics, AdvancedGraphics* advancedGraphics) :
        display(display),
        print(print),
        graphics(graphics),
        advancedGraphics(advancedGraphics) {}

    Display& getDisplay();
    Print& getPrint();
    Graphics& getGraphics();
    AdvancedGraphics& getAdvancedGraphics();

private:
    Display* display;
    Print* print;
    Graphics* graphics;
    AdvancedGraphics* advancedGraphics;
};