#include "gradient.h"
#include <stdio.h>

// create a global instance of the lookup tables
color_t colorLUT[MAX_COLOR_DIFF + 1];

/**
 * @brief Construct a new Advanced Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
gradient_obj::gradient_obj(color_t *frameBuffer, display_obj *display_ptr)
{
    this->frameBuffer = frameBuffer;
    this->display_ptr = display_ptr;
    this->theta = 0;
}

/**
 * @brief Fill the display with a color gradient
 * @param lightColor color to start with
 * @param darkColor color to end with
 * @param area The area to fill
 * @warning The gradients will not properly work if the light and dark colors are not properly selected
*/
void gradient_obj::fillGradient(color startColor, color endColor, rect area)
{
    this->fillGradient(startColor, endColor, area.x(), area.y());
}

/**
 * @brief Fill the display with a color gradient
 * @param lightColor color to start with
 * @param darkColor color to end with
 * @param start Start point
 * @param end End point
 * @note The start and end points are only used to find the direction of the gradient, it will still fill the entire display!
 * @warning The gradients will not properly work if the light and dark colors are not properly selected
*/
void gradient_obj::fillGradient(color lightColor, color darkColor, point start, point end)
{
    // check if the start and end Points are the same
    if(start == end)
    {
        color_t startColorWord = lightColor.toWord();
        uint32_t totalPixels = this->display_ptr->getWidth() * this->display_ptr->getHeight();
        for(int32_t i = 0; i < totalPixels; i++)
            this->frameBuffer[i] = startColorWord;

        return;
    }

    // calculate the direction of the gradient
    int32_t deltaX = end.x - start.x;
    int32_t deltaY = end.y - start.y;
    int32_t magnitudeSquared = (deltaX * deltaX + deltaY * deltaY);
    int32_t gradientLengthFP = ((int64_t)magnitudeSquared << FIXED_POINT_SCALE_HIGH_RES_BITS) / magnitudeSquared;

    // find the maximum difference between the color components
    int32_t dr = darkColor.r - lightColor.r;
    int32_t dg = darkColor.g - lightColor.g;
    int32_t db = darkColor.b - lightColor.b;
    int32_t colorDistance = (dr * dr) + (dg * dg) + (db * db);

#if defined(LCD_COLOR_DEPTH_1)
    uint32_t maxDistance = 3;
#elif defined(LCD_COLOR_DEPTH_8)
    uint32_t maxDistance = 7*7 + 7*7 + 3*3;
#elif defined(LCD_COLOR_DEPTH_16)
    uint32_t maxDistance = 31*31 + 63*63 + 31*31;
#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
    uint32_t maxDistance = (255 * 255) * 3;
#endif

    // loop through each position in the gradient
    for(int32_t i = 0; i < MAX_COLOR_DIFF; i++)
    {
        int32_t gammaIndex = ((int64_t)i * i) / MAX_COLOR_DIFF;
        colorLUT[i] = (lightColor.interp(darkColor, gammaIndex, MAX_COLOR_DIFF)).toWord();
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
            int32_t position = ((int64_t)dotProduct * (MAX_COLOR_DIFF - 1) * magnitudeInverse) >> FIXED_POINT_SCALE_HIGH_RES_BITS;

            // clamp the position within the valid range
            position = (position < 0) ? 0 : (position > MAX_COLOR_DIFF) ? MAX_COLOR_DIFF : position;

            // draw the pixel
			this->frameBuffer[x + y * this->display_ptr->getWidth()] = colorLUT[position];
        }
    }
}

/**
 * @brief Draw a circle gradient
 * @param c Circle to draw
 * @param rotationSpeed The speed at which the gradient rotates
 * @param lightColor The color to start the gradient with
 * @param darkColor The color to end the gradient with
 */
void gradient_obj::drawRotCircleGradient(circle c, int32_t rotationSpeed, color lightColor, color darkColor)
{
    this->drawRotCircleGradient(c.getCenter(), c.getRadius(), rotationSpeed, lightColor, darkColor);
}

/**
 * @brief Draw a circle gradient
 * @param center The center of the circle
 * @param radius The radius of the circle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param lightColor The color to start the gradient with
 * @param darkColor The color to end the gradient with
*/
void gradient_obj::drawRotCircleGradient(point center, int32_t radius, int32_t rotationSpeed, 
    color lightColor, color darkColor)
{
    // Ensure theta stays positive for the modulo
    this->theta = (this->theta + rotationSpeed + 360) % 360;

    int32_t cosTheta = icos(this->theta);
    int32_t sinTheta = isin(this->theta);

    // Calculate the scaled offsets
    // radius * 2^16 can be up to 2,147,483,647 (max int32) if radius is ~32767
    // So we are safe with int32_t here for standard screen sizes.
    int32_t dx = (radius * cosTheta) >> 16;
    int32_t dy = (radius * sinTheta) >> 16;

    // Now apply to the center pixel coordinates
    point rotGradStart = point(center.x - dx, center.y - dy);
    point rotGradEnd   = point(center.x + dx, center.y + dy);

    this->fillGradient(lightColor, darkColor, rotGradStart, rotGradEnd);
}

/**
 * @brief Draw a rectangle gradient
 * @param center The center of the rectangle
 * @param width The width of the rectangle
 * @param height The height of the rectangle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param lightColor The color to start the gradient with
 * @param darkColor The color to end the gradient with
*/
void gradient_obj::drawRotRectGradient(point center, int32_t width, int32_t height, 
    int32_t rotationSpeed, color lightColor, color darkColor)
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

    this->fillGradient(lightColor, darkColor, rotGradStart, rotGradEnd);
}

/**
 * @brief Draw a rectangle gradient
 * @param center The center of the rectangle
 * @param area The area to fill
 * @param rotationSpeed The speed at which the gradient rotates
 * @param lightColor The color to start the gradient with
 * @param darkColor The color to end the gradient with
 */
void gradient_obj::drawRotRectGradient(point center, rect area, int32_t rotationSpeed, 
    color lightColor, color darkColor)
{
    this->drawRotRectGradient(center, area.width(), area.height(), rotationSpeed, lightColor, darkColor);
}