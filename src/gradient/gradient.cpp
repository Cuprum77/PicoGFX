#include "gradient.h"
#include <stdio.h>

// create a global instance of the lookup tables
uint16_t colorLUT[MAX_COLOR_DIFF + 1];

/**
 * @brief Construct a new Advanced Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
gradient_obj::gradient_obj(uint16_t *frameBuffer, display_obj *display_ptr)
{
    this->frameBuffer = frameBuffer;
    this->display_ptr = display_ptr;
    this->theta = 0;
}

/**
 * @brief Fill the display with a color gradient
 * @param startColor color to start with
 * @param endColor color to end with
 * @param area The area to fill
*/
void gradient_obj::fillGradient(color startColor, color endColor, rect area)
{
    this->fillGradient(startColor, endColor, area.x(), area.y());
}

/**
 * @brief Fill the display with a color gradient
 * @param startColor color to start with
 * @param endColor color to end with
 * @param start Start point
 * @param end End point
 * @note The start and end points are only used to find the direction of the gradient, it will still fill the entire display!
*/
void gradient_obj::fillGradient(color startColor, color endColor, point start, point end)
{
    // check if the start and end Points are the same
    if(start == end)
    {
        uint16_t startColor16 = startColor.toWord();
        uint32_t totalPixels = this->display_ptr->getWidth() * this->display_ptr->getHeight();
        for(int32_t i = 0; i < totalPixels; i++)
            this->frameBuffer[i] = startColor16;

        return;
    }

    // calculate the direction of the gradient
    int32_t deltaX = end.x - start.x;
    int32_t deltaY = end.y - start.y;
    int32_t magnitudeSquared = (deltaX * deltaX + deltaY * deltaY);

    // find the maximum difference between the color components
    int32_t dr = iabs(endColor.r - startColor.r);
    int32_t dg = iabs(endColor.g - startColor.g);
    int32_t db = iabs(endColor.b - startColor.b);
    int32_t maxDiff = imax(dr, imax(dg, db));

    // create the lookup tables based on the maximum difference
    uint32_t numPositions = maxDiff + 1;

    // loop through each position in the gradient
    for(int32_t i = 0; i < numPositions; i++)
    {
        // interpolate the color components based on the position and add them to the lookup tables
        unsigned char r = (((endColor.r - startColor.r) * i) / maxDiff + startColor.r) & 0x1f;
        unsigned char g = (((endColor.g - startColor.g) * i) / maxDiff + startColor.g) & 0x3f;
        unsigned char b = (((endColor.b - startColor.b) * i) / maxDiff + startColor.b) & 0x1f;
		colorLUT[i] = (r << 11) | (g << 5) | b;
    }

    // precalculate the divisor
    int32_t magnitudeInverse = (FIXED_POINT_SCALE_HIGH_RES + (magnitudeSquared / 2)) / magnitudeSquared;

    // loop through each pixel in the buffer
    for(int32_t x = 0; x < this->display_ptr->getWidth(); x++)
    {
        for (int32_t y = 0; y < this->display_ptr->getHeight(); y++)
        {
            // calculate the vector from the start to the current pixel
            int32_t vectorX = x - start.x;
            int32_t vectorY = y - start.y;

            // calculate the distance along the gradient direction
            int32_t dotProduct = (vectorX * deltaX + vectorY * deltaY);
            //int32_t position = (dotProduct * maxDiff) / magnitudeSquared;
            int32_t position = ((dotProduct * maxDiff) * magnitudeInverse);
            position >>= FIXED_POINT_SCALE_HIGH_RES_BITS;

            // clamp the position within the valid range
            position = (position < 0) ? 0 : (position > maxDiff) ? maxDiff : position;

            // draw the pixel
			this->frameBuffer[x + y * this->display_ptr->getWidth()] = colorLUT[position];
        }
    }
}

/**
 * @brief Draw a circle gradient
 * @param c Circle to draw
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
 */
void gradient_obj::drawRotCircleGradient(circle c, int32_t rotationSpeed, color start, color end)
{
    this->drawRotCircleGradient(c.getCenter(), c.getRadius(), rotationSpeed, start, end);
}

/**
 * @brief Draw a circle gradient
 * @param center The center of the circle
 * @param radius The radius of the circle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void gradient_obj::drawRotCircleGradient(point center, int32_t radius, int32_t rotationSpeed, color start, color end)
{
    this->theta += rotationSpeed;
    this->theta = this->theta % 360;

    int32_t cosTheta = icos(this->theta);
    int32_t sinTheta = isin(this->theta);

    point rotGradStart = point(
        center.x - (radius * cosTheta),
        center.y - (radius * sinTheta)
    );
    rotGradStart >>= SIN_MULTIPLIER_BITS;
    
    point rotGradEnd = point(
        center.x + (radius * cosTheta),
        center.y + (radius * sinTheta)
    );
    rotGradEnd >>= SIN_MULTIPLIER_BITS;

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
void gradient_obj::drawRotRectGradient(point center, int32_t width, int32_t height, int32_t rotationSpeed, color start, color end)
{
    this->theta += rotationSpeed;
    this->theta = this->theta % 360;
    point rotGradStart, rotGradEnd;

    // follow the quadrants of the unit circle
    if(this->theta < this->firstQuadrant)
        rotGradStart = point(
            center.x + (width >> 1),
            center.y - (height >> 1) + (this->theta * height / this->firstQuadrant)
        );
    else if(this->theta < this->secondQuadrant)
        rotGradStart = point(
            center.x + (width >> 1) - ((this->theta - this->firstQuadrant) * width / this->firstQuadrant),
            center.y + (height >> 1)
        );
    else if(this->theta < this->thirdQuadrant)
        rotGradStart = point(
            center.x - (width >> 1),
            center.y + (height >> 1) - ((this->theta - this->secondQuadrant) * height / this->firstQuadrant)
        );
    else
        rotGradStart = point(
            center.x - (width >> 1) + ((this->theta - this->thirdQuadrant) * width / this->firstQuadrant),
            center.y - (height >> 1)
        );

    // create the end point by making the start point the opposite corner
    rotGradEnd = point(
        center.x - (rotGradStart.x - center.x),
        center.y - (rotGradStart.y - center.y)
    );

    this->fillGradient(start, end, rotGradStart, rotGradEnd);
}

/**
 * @brief Draw a rectangle gradient
 * @param center The center of the rectangle
 * @param area The area to fill
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
 */
void gradient_obj::drawRotRectGradient(point center, rect area, int32_t rotationSpeed, color start, color end)
{
    this->drawRotRectGradient(center, area.width(), area.height(), rotationSpeed, start, end);
}