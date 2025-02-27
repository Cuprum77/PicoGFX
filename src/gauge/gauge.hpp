#pragma once

#include "graphics.hpp"

// Constants
#define DIAL_ANGLE 230  // Should be between 0 and 360 degrees
#define VALUE_BUFFER_SIZE 16

typedef enum
{
    DialSimple,     // Simple dial with a needle
    DialSimple2,    // Simple dial with only two colors
} dialGaugeType_t;

class dialGauge
{
public:
    // Constructor
    dialGauge(graphics* graphics, int screenWidth, int screenHeight, point center, int radius, 
        int minValue, int maxValue, color* valueColors, size_t numberOfValueColors, dialGaugeType_t type, int angle = DIAL_ANGLE);

    // Add extra functionality
    void setNeedleColor(color value);
    void attachBackgroundColor(color value);

    // Draw a line on the dial at a given value
    void drawLine(int value, int width, color color);

    // Update the dial value
	void update(int value);

private:
    // Display variables
    graphics* graphics_ptr = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    size_t totalPixels = 0;

    // Dial variables
    point center = { 0,0 };
    int angle = 0;
    int halfAngle = 0;
    int radius = 0;
    int innerRadius = 0;
    int minValue = 0;
    int maxValue = 0;
    color* valueColors = nullptr;
    size_t numberOfValueColors = 0;
    dialGaugeType_t type = dialGaugeType_t::DialSimple;
    color needleColor = colors::white;
    color background = colors::black;
    bool hasBackground = false;

    // Private helper functions
    void drawNeedle(int value);
    void drawSimpleDial(int value);
    void drawSimpleDial2(int value);
};