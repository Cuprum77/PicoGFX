#pragma once

#include "HardwareSPI.hpp"
#include "Display.hpp"
#include "Graphics.hpp"
#include "Print.hpp"
#include "Gradients.hpp"
#include "Encoder.h"

class PicoGFX {
public:
    PicoGFX(Display* display, Print* print, Graphics* graphics, Gradients* gradients, Encoder* encoder) :
        display(display),
        print(print),
        graphics(graphics),
        gradients(gradients) {
        fillLookupTables();
    }

    Display& getDisplay();
    Print& getPrint();
    Graphics& getGraphics();
    Gradients& getGradients();
    Encoder& getEncoder();

private:
    Display* display;
    Print* print;
    Graphics* graphics;
    Gradients* gradients;
    Encoder* encoder;
};