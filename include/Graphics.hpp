#pragma once

#include "Display.hpp"

class Graphics
{
public:
    void fillGradientCool(Color startColor, Color endColor, Point start, Point end);
    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rectangle rect, Color color = Colors::White);
    void drawRectangle(Point center, uint width, uint height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);
    void drawCircle(Point center, uint radius, Color color = Colors::White);
    void drawFilledCircle(Point center, uint radius, Color color = Colors::White);
    void drawArc(Point center, uint radius, uint start_angle, uint end_angle, Color color = Colors::White);
    void drawFilledArc(Point center, uint radius, uint start_angle, uint end_angle, uint outer_radius, uint inner_radius, Color color = Colors::White);

    void drawBitmap(const unsigned char* bitmap, uint width, uint height);
    void drawBitmap(const unsigned short* bitmap, uint width, uint height);
    void drawBitmap(Color* bitmap, uint width, uint height);
};