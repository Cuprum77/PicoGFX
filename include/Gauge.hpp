#pragma once

#include "Graphics.hpp"

// Constants
#define DIAL_ANGLE 230  // Should be between 0 and 360 degrees
#define VALUE_BUFFER_SIZE 16

typedef enum
{
    DialSimple,     // Simple dial with a needle
    DialSimple2,    // Simple dial with only two colors
} DialGaugeType_t;

class DialGauge
{
public:
    // Constructor
    DialGauge(Graphics* graphics, int screenWidth, int screenHeight, Point center, int radius, 
        int minValue, int maxValue, Color* valueColors, size_t numberOfValueColors, DialGaugeType_t type, int angle = DIAL_ANGLE);

    // Add extra functionality
    void setNeedleColor(Color value);
    void attachBackgroundColor(Color value);

    // Draw a line on the dial at a given value
    void drawLine(int value, int width, Color color);

    // Update the dial value
	void update(int value);

private:
    // Display variables
    Graphics* graphics = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    size_t totalPixels = 0;

    // Dial variables
    Point center = { 0,0 };
    int angle = 0;
    int halfAngle = 0;
    int radius = 0;
    int innerRadius = 0;
    int minValue = 0;
    int maxValue = 0;
    Color* valueColors = nullptr;
    size_t numberOfValueColors = 0;
    DialGaugeType_t type = DialGaugeType_t::DialSimple;
    Color needleColor = Colors::White;
    Color background = Colors::Black;
    bool hasBackground = false;

    // Private helper functions
    void drawNeedle(int value);
    void drawSimpleDial(int value);
    void drawSimpleDial2(int value);
};