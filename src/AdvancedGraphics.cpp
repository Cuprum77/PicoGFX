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
 * @param center The center of the circle
 * @param radius The radius of the circle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void AdvancedGraphics::drawRotCircleGradient(Point center, int radius, int rotationSpeed, Color start, Color end)
{
    this->theta += rotationSpeed;
    this->theta = this->theta % numAngles;

    int cosTheta = fixedPointCosTable[theta];
    int sinTheta = fixedPointSinTable[theta];

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
 * @brief Draw a rectangle gradient
 * @param center The center of the rectangle
 * @param width The width of the rectangle
 * @param height The height of the rectangle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void AdvancedGraphics::drawRotRectGradient(Point center, int width, int height, int rotationSpeed, Color start, Color end)
{
    this->theta += rotationSpeed;
    this->theta = this->theta % numAngles;
    Point rotGradStart, rotGradEnd;

    // follow the quadrants of the unit circle
    if(this->theta < this->firstQuadrant)
        rotGradStart = Point(
            center.x + (width >> 1),
            center.y - (height >> 1) + (this->theta * height / this->firstQuadrant)
        );
    else if(this->theta < this->secondQuadrant)
        rotGradStart = Point(
            center.x + (width >> 1) - ((this->theta - this->firstQuadrant) * width / this->firstQuadrant),
            center.y + (height >> 1)
        );
    else if(this->theta < this->thirdQuadrant)
        rotGradStart = Point(
            center.x - (width >> 1),
            center.y + (height >> 1) - ((this->theta - this->secondQuadrant) * height / this->firstQuadrant)
        );
    else
        rotGradStart = Point(
            center.x - (width >> 1) + ((this->theta - this->thirdQuadrant) * width / this->firstQuadrant),
            center.y - (height >> 1)
        );

    // create the end point by making the start point the opposite corner
    rotGradEnd = Point(
        center.x - (rotGradStart.x - center.x),
        center.y - (rotGradStart.y - center.y)
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