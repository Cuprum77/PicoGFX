#pragma once

#include "Graphics.hpp"
#include "Print.hpp"
#include "Gradients.hpp"
#include "Encoder.h"
#include "Gauge.hpp"

class PicoGFX {
public:
    PicoGFX(Print* print, Graphics* graphics, Gradients* gradients, Encoder* encoder) :
        print(print),
        graphics(graphics),
        gradients(gradients) {
        fillLookupTables();
    }

    Print& getPrint();
    Graphics& getGraphics();
    Gradients& getGradients();
    Encoder& getEncoder();

    Print* getPrintPtr();
    Graphics* getGraphicsPtr();
    Gradients* getGradientsPtr();
    Encoder* getEncoderPtr();

private:
    Print* print;
    Graphics* graphics;
    Gradients* gradients;
    Encoder* encoder;
};