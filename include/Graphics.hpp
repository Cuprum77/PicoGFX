#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "GfxMath.h"

class Graphics
{
public:
    Graphics(unsigned short* frameBuffer, display_config_t* config);

    void fill(Color color);

    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawLineAntiAliased(Point start, Point end, Color color = Colors::White);
    void drawLineThickAntiAliased(Point start, Point end, int thickness, Color color = Colors::White);

	void drawTriangle(Point p1, Point p2, Point p3, Color color = Colors::White);
	void drawFilledTriangle(Point p1, Point p2, Point p3, Color color = Colors::White);

    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rect rect, Color color = Colors::White);
    void drawRectangle(Point center, unsigned int width, unsigned int height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);

    void drawPolygon(Point* points, size_t numberOfPoints, Color color = Colors::White);
    void drawFilledPolygon(Point* points, size_t numberOfPoints, Color color = Colors::White);

    void drawCircle(Point center, unsigned int radius, Color color = Colors::White);
    void drawFilledCircle(Point center, unsigned int radius, Color color = Colors::White);

    void drawArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, Color color = Colors::White);
    void drawFilledDualArc(Point center, int innerRadius, int outerRadius, int startAngle, int endAngle, Color color);

    void drawBitmap(const unsigned char* bitmap, unsigned int width, unsigned int height);
    void drawBitmap(const unsigned short* bitmap, unsigned int width, unsigned int height);

    void antiAliasingFilter(void);
private:
    unsigned short* frameBuffer;
    display_config_t* config;
    size_t totalPixels;

    void setPixelBlend(int x, int y, unsigned short background, unsigned char alpha);
};