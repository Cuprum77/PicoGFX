#pragma once

#include "color.h"
#include "structs.h"
#include "shapes.hpp"
#include "gfxmath.h"

class gradient
{
public:
    gradient(uint16_t* frameBuffer, display_config_t* config);

    void fillGradient(color startColor, color endColor, point start, point end);
    void fillGradient(color startColor, color endColor, rect area);
    void drawRotCircleGradient(point center, int32_t radius, int32_t rotationSpeed, color start, color end);
    void drawRotRectGradient(point center, int32_t width, int32_t height, int32_t rotationSpeed, color start, color end);
    void drawRotRectGradient(point center, rect area, int32_t rotationSpeed, color start, color end);
private:
    uint16_t* frameBuffer;
    display_config_t* config;
    size_t totalPixels;

    uint32_t theta; // The angle of the rotating gradient
    const int32_t firstQuadrant = 90;
    const int32_t secondQuadrant = 180;
    const int32_t thirdQuadrant = 270;
};