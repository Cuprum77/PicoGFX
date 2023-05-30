#include "AdvancedGraphics.hpp"

/**
 * @brief Construct a new Advanced Graphics object
*/
AdvancedGraphics::AdvancedGraphics(spi_inst_t* spi, Display_Pins pins, 
    Display_Params params, display_type_t type, bool dimming) : Display(spi, pins, params, type, dimming)
{
    this->theta = 0;
    this->fillLookupTables();
}

/**
 * @brief Draw a circle gradient
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void AdvancedGraphics::drawRotCircleGradient(Point center, int radius, int rotationSpeed, Color start, Color end)
{
    this->theta += rotationSpeed;

    int cosTheta = fixedPointCos(this->theta);
    int sinTheta = fixedPointSin(this->theta);

    Point rotGradStart = Point(
        center.x - (radius * cosTheta) / fixedPointScale,
        center.y - (radius * sinTheta) / fixedPointScale
    );
    Point rotGradEnd = Point(
        center.x + (radius * cosTheta) / fixedPointScale,
        center.y + (radius * sinTheta) / fixedPointScale
    );

    this->fillGradient(start, end, rotGradStart, rotGradEnd);
}

/**
 * @brief Draw an eclipse gradient
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void AdvancedGraphics::drawRotEllipseGradient(Point center, int width, int height, int rotationSpeed, Color start, Color end)
{
    this->theta += rotationSpeed;

    Point rotGradStart = Point(
        center.x + (width * fixedPointCos(this->theta)) / fixedPointScale,
        center.y + (height * fixedPointSin(this->theta)) / fixedPointScale
    );
    Point rotGradEnd = Point(
        center.x + (width * fixedPointCos(this->theta + halfNumAngles)) / fixedPointScale,
        center.y + (height * fixedPointSin(this->theta + halfNumAngles)) / fixedPointScale
    );

    this->fillGradient(start, end, rotGradStart, rotGradEnd);
}


/**
 * @private
 * @brief Fill the lookup tables for the sine and cosine functions
*/
void AdvancedGraphics::fillLookupTables()
{
    for (int angle = 0; angle < numAngles; ++angle)
    {
        // Convert angle to radians
        double radians = angle * PI / halfNumAngles;

        // Calculate cosine and sine using math functions
        double cosValue = cos(radians);
        double sinValue = sin(radians);

        // Convert to fixed-point representation
        fixedPointCosTable[angle] = static_cast<int>(cosValue * fixedPointScale);
        fixedPointSinTable[angle] = static_cast<int>(sinValue * fixedPointScale);
    }
}

/**
 * @private
 * @brief Get the cosine of an angle
 * @param theta The angle to get the cosine of
 * @return The cosine of the angle
*/
int AdvancedGraphics::fixedPointCos(int theta)
{
    // Ensure angle is within the range 0-359
    theta = theta % numAngles;

    return fixedPointCosTable[theta];
}

/**
 * @private
 * @brief Get the sine of an angle
 * @param theta The angle to get the sine of
 * @return The sine of the angle
*/
int AdvancedGraphics::fixedPointSin(int theta)
{
    // Ensure angle is within the range 0-359
    theta = theta % numAngles;

    return fixedPointSinTable[theta];
}