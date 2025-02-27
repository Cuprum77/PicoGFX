#pragma once

#include "color.h"
#include "structs.h"
#include "shapes.hpp"
#include "gfxmath.h"
#include <stdint.h>

class Graphics
{
public:
    Graphics(uint16_t* frameBuffer, display_config_t* config);

    void fill(Color color);

    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawLineAntiAliased(Point start, Point end, Color color = Colors::White);
    void drawLineThickAntiAliased(Point start, Point end, uint32_t thickness, Color color = Colors::White);

	void drawTriangle(Point p1, Point p2, Point p3, Color color = Colors::White);
	void drawFilledTriangle(Point p1, Point p2, Point p3, Color color = Colors::White);

    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rect rect, Color color = Colors::White);
    void drawRectangle(Point center, uint32_t width, uint32_t height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);

    void drawPolygon(Point* points, size_t numberOfPoints, Color color = Colors::White);
    void drawFilledPolygon(Point* points, size_t numberOfPoints, Color color = Colors::White);

    void drawCircle(Point center, uint32_t radius, Color color = Colors::White, uint32_t thickness = 1);
    void drawFilledCircle(Point center, uint32_t radius, Color color = Colors::White);
    void drawFilledCircleWithStroke(Point center, uint32_t radius, Color fillColor, Color strokeColor, uint32_t strokeThickness);

    void drawArc(Point center, uint32_t radius, uint32_t start_angle, uint32_t end_angle, Color color = Colors::White);
    void drawFilledDualArc(Point center, uint32_t innerRadius, uint32_t outerRadius, uint32_t startAngle, uint32_t endAngle, Color color);

    void drawBitmap(const uint8_t* bitmap, uint32_t width, uint32_t height);
    void drawBitmap(const uint16_t* bitmap, uint32_t width, uint32_t height);

    void antiAliasingFilter(void);
private:
    uint16_t* frameBuffer;
    display_config_t* config;
    size_t totalPixels;

    inline void setPixel(uint32_t x, uint32_t y, uint16_t color) { this->frameBuffer[x + y * this->config->width] = color; }
    void setPixelBlend(uint32_t x, uint32_t y, uint16_t background, uint8_t alpha);
    void drawCircle1(Point center, uint32_t radius, Color color);
    void drawCircle2(Point center, uint32_t radius, Color color, uint32_t thickness = 2);
    void drawCircleXLine(uint32_t x1, uint32_t x2, uint32_t y, Color color);
    void drawCircleYLine(uint32_t x, uint32_t y1, uint32_t y2, Color color);
};