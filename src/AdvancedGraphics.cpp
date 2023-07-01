#include "AdvancedGraphics.hpp"

// create a global instance of the lookup tables
int rLUT[MAX_COLOR_DIFF + 1];
int gLUT[MAX_COLOR_DIFF + 1];
int bLUT[MAX_COLOR_DIFF + 1];

/**
 * @brief Construct a new Advanced Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
AdvancedGraphics::AdvancedGraphics(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->params = params;
    this->totalPixels = params.width * params.height;
    this->theta = 0;
    this->fillLookupTables();
}

/**
 * @brief Fill the display with a color gradient
 * @param startColor Color to start with
 * @param endColor Color to end with
 * @param start Start Point
 * @param end End Point
 * @note The start and end points are only used to find the direction of the gradient, it will still fill the entire display!
*/
void AdvancedGraphics::fillGradient(Color startColor, Color endColor, Point start, Point end)
{
    // check if the start and end Points are the same
    if(start == end)
    {
        unsigned short startColor16 = startColor.to16bit();
        for(int i = 0; i < this->totalPixels; i++)
            this->frameBuffer[i] = startColor16;

        return;
    }

    // calculate the direction of the gradient
    int deltaX = end.X() - start.X();
    int deltaY = end.Y() - start.Y();
    int magnitudeSquared = (deltaX * deltaX + deltaY * deltaY);

    // find the maximum difference between the color components
    int dr = abs(endColor.r - startColor.r);
    int dg = abs(endColor.g - startColor.g);
    int db = abs(endColor.b - startColor.b);
    int maxDiff = std::max(dr, std::max(dg, db));

    // create the lookup tables based on the maximum difference
    int numPositions = maxDiff + 1;

    // loop through each position in the gradient
    for(int i = 0; i < numPositions; i++)
    {
        // interpolate the color components based on the position and add them to the lookup tables
        rLUT[i] = ((endColor.r - startColor.r) * i) / maxDiff + startColor.r;
        gLUT[i] = ((endColor.g - startColor.g) * i) / maxDiff + startColor.g;
        bLUT[i] = ((endColor.b - startColor.b) * i) / maxDiff + startColor.b;
    }

    // loop through each pixel in the buffer
    for(int i = 0; i < this->totalPixels; i++)
    {
        // calculate the position along the gradient direction
        int x = i % this->params.width;
        int y = i / this->params.width;

        // calculate the vector from the start to the current pixel
        int vectorX = x - start.X();
        int vectorY = y - start.Y();

        // calculate the distance along the gradient direction
        int dotProduct = (vectorX * deltaX + vectorY * deltaY);
        int position = (dotProduct * maxDiff) / magnitudeSquared;  // Scale position to 0-100 range

        // clamp the position within the valid range
        position = (position < 0) ? 0 : (position > maxDiff) ? maxDiff : position;

        // get the interpolated color components from the lookup tables and create the color
        Color color(
            rLUT[position], 
            gLUT[position], 
            bLUT[position]
        );

        // draw the pixel
        this->frameBuffer[i] = color.to16bit();
    }
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