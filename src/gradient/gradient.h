#pragma once

#include "color.h"
#include "shapes.h"
#include "gfxmath.h"
#include "display.h"
#include "lcd_config.h"

class gradient
{
public:
    gradient(uint16_t *frameBuffer, display *display_ptr = nullptr);

    void fillGradient(color startColor, color endColor, rect area);
    void fillGradient(color startColor, color endColor, point start, point end);
    void drawRotCircleGradient(circle c, int32_t rotationSpeed, color start, color end);
    void drawRotCircleGradient(point center, int32_t radius, int32_t rotationSpeed, color start, color end);
    void drawRotRectGradient(point center, int32_t width, int32_t height, int32_t rotationSpeed, color start, color end);
    void drawRotRectGradient(point center, rect area, int32_t rotationSpeed, color start, color end);
private:
    uint16_t *frameBuffer;
    display *display_ptr;
    size_t totalPixels;

    uint32_t theta; // The angle of the rotating gradient
    const int32_t firstQuadrant = 90;
    const int32_t secondQuadrant = 180;
    const int32_t thirdQuadrant = 270;
};