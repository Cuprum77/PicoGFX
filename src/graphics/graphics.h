#pragma once

#include "color.h"
#include "display.h"
#include "shapes.h"
#include "gfxmath.h"
#include <stdint.h>

class graphics
{
public:
    graphics(uint16_t *frameBuffer, display *display_ptr = nullptr);

    void fill(color color);
    void fill(uint16_t color);

    void testPattern(void);

    void drawLine(point start, point end, color color = colors::white);
    void drawLineAntiAliased(point start, point end, color color = colors::white);
    void drawLineThickAntiAliased(point start, point end, uint32_t thickness, color color = colors::white);

	void drawTriangle(point p1, point p2, point p3, color color = colors::white);
	void drawFilledTriangle(point p1, point p2, point p3, color color = colors::white);

    void drawRectangle(point start, point end, color color = colors::white);
    void drawRectangle(rect rect, color color = colors::white);
    void drawRectangle(point center, uint32_t width, uint32_t height, color color = colors::white);
    void drawFilledRectangle(point start, point end, color color = colors::white);

    void drawPolygon(point *points, size_t numberOfPoints, color color = colors::white);
    void drawFilledPolygon(point *points, size_t numberOfPoints, color color = colors::white);

    void drawCircle(circle c, color color = colors::white, uint32_t thickness = 1);
    void drawCircle(point center, uint32_t radius, color color = colors::white, uint32_t thickness = 1);
    void drawFilledCircle(circle c, color color = colors::white);
    void drawFilledCircle(point center, uint32_t radius, color color = colors::white);
    void drawFilledCircleWithStroke(circle c, color fillColor, color strokeColor, uint32_t strokeThickness);
    void drawFilledCircleWithStroke(point center, uint32_t radius, color fillColor, color strokeColor, uint32_t strokeThickness);

    void drawArc(point center, uint32_t radius, uint32_t start_angle, uint32_t end_angle, color color = colors::white);
    void drawFilledDualArc(point center, uint32_t innerRadius, uint32_t outerRadius, uint32_t startAngle, uint32_t endAngle, color color);

    void drawBitmap(const uint8_t *bitmap, uint32_t width, uint32_t height);
    void drawBitmap(const uint16_t *bitmap, uint32_t width, uint32_t height);
    void drawBitmap(const uint8_t *bitmap, uint32_t width, uint32_t height, bool center);
    void drawBitmap(const uint16_t *bitmap, uint32_t width, uint32_t height, bool center);
    void drawBitmap(const uint8_t *bitmap, uint32_t width, uint32_t height, point start);
    void drawBitmap(const uint16_t *bitmap, uint32_t width, uint32_t height, point start);

    void addBayerFilter(void);
    void addFloydSteinbergDithering(void);
    void addAntiAliasingFilter(void);
    void addBlur(void);
    void addBlur(rect area);
private:
    uint16_t *frameBuffer;
    display *display_ptr;
    uint32_t width;
    uint32_t height;
    bool inverseColors = false;

    const uint8_t bayerMatrix[16] = {
        0, 8, 2, 10,
        12, 4, 14, 6,
        3, 11, 1, 9,
        15, 7, 13, 5
    };

    inline void setPixel(uint32_t x, uint32_t y, uint16_t color) { this->frameBuffer[x + y * this->display_ptr->getWidth()] = color; }
    void setPixelBlend(uint32_t x, uint32_t y, uint16_t background, uint8_t alpha);
    void drawCircle1(point center, uint32_t radius, color color);
    void drawCircle2(point center, uint32_t radius, color color, uint32_t thickness = 2);
    void drawCircleXLine(uint32_t x1, uint32_t x2, uint32_t y, color color);
    void drawCircleYLine(uint32_t x, uint32_t y1, uint32_t y2, color color);
};