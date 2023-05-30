#pragma once

#include "Display.hpp"

#define numAngles 3600  // decides how many angles to precompute, additional zeroes for more precision
#define halfNumAngles 1800 // half of numAngles, this has to be exactly half and precomputed to speed up calculations
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

class AdvancedGraphics : public Display
{
public:
    AdvancedGraphics(spi_inst_t* spi, Display_Pins pins, 
        Display_Params params, display_type_t type = ST7789, bool dimming = false);

    void drawRotCircleGradient(Point center, int radius, int rotationSpeed, Color start, Color end);
    void drawRotEllipseGradient(Point center, int width, int height, int rotationSpeed, Color start, Color end);
private:
    int theta; // The angle of the rotating gradient
    const int fixedPointScale = 4096; // 2^12

    int fixedPointCosTable[numAngles];
    int fixedPointSinTable[numAngles];

    void fillLookupTables();
    int fixedPointCos(int theta);
    int fixedPointSin(int theta);
};