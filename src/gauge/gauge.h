#pragma once

#include "graphics.h"
#include "lcd_config.h"

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
    dialGauge(graphics *graphics, int32_t screenWidth, int32_t screenHeight, point center, int32_t radius, 
        int32_t minValue, int32_t maxValue, color *valueColors, size_t numberOfValueColors, dialGaugeType_t type, int32_t angle = DIAL_ANGLE);

    // Add extra functionality
    void setNeedleColor(color value);
    void attachBackgroundColor(color value);

    // Draw a line on the dial at a given value
    void drawLine(int32_t value, int32_t width, color color);

    // Update the dial value
	void update(int32_t value);

private:
    // Display variables
    graphics *graphics_ptr = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    size_t totalPixels = 0;

    // Dial variables
    point center = { 0,0 };
    int32_t angle = 0;
    int32_t halfAngle = 0;
    int32_t radius = 0;
    int32_t innerRadius = 0;
    int32_t minValue = 0;
    int32_t maxValue = 0;
    color *valueColors = nullptr;
    size_t numberOfValueColors = 0;
    dialGaugeType_t type = dialGaugeType_t::DialSimple;
    color needleColor = colors::white;
    color background = colors::black;
    bool hasBackground = false;

    // Private helper functions
    void drawNeedle(int32_t value);
    void drawSimpleDial(int32_t value);
    void drawSimpleDial2(int32_t value);
    point getPointOnCircle(point p, int32_t radius, int32_t angle);
};