#pragma once

#include "Color.hpp"
#include "Structs.hpp"

#define numAngles 3600  // decides how many angles to precompute, additional zeroes for more precision
#define halfNumAngles 1800 // half of numAngles, this has to be exactly half and precomputed to speed up calculations
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

class AdvancedGraphics
{
public:
    AdvancedGraphics(unsigned short* frameBuffer, Display_Params params);

    void fillGradient(Color startColor, Color endColor, Point start, Point end);
    void drawRotCircleGradient(Point center, int radius, int rotationSpeed, Color start, Color end);
    void drawRotRectGradient(Point center, int width, int height, int rotationSpeed, Color start, Color end);
private:
    unsigned short* frameBuffer;
    Display_Params params;
    size_t totalPixels;

    unsigned int theta; // The angle of the rotating gradient
    const int fixedPointScale = 4096; // 2^12
    const int firstQuadrant = numAngles / 4;
    const int secondQuadrant = numAngles / 2;
    const int thirdQuadrant = numAngles * 3 / 4;

    int fixedPointCosTable[numAngles];
    int fixedPointSinTable[numAngles];

    void fillLookupTables();
};