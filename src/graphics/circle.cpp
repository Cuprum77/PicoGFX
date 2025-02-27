#include "graphics.hpp"

/**
 * @brief Draw a circle on the display
 * @param center Center point
 * @param radius Radius of the circle
 * @param color color to draw in
*/
void graphics::drawCircle(point center, uint32_t radius, color color, uint32_t thickness)
{
    if (thickness == 0 || thickness == 1)
        this->drawCircle1(center, radius, color);
    else
        this->drawCircle2(center, radius, color, thickness);
}

/**
 * @brief Draw a filled circle on the display
 * @param center Center point
 * @param radius Radius of the circle
 * @param color color to draw in
*/
void graphics::drawFilledCircle(point center, uint32_t radius, color color)
{
    // Uses a modified Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int32_t x0 = center.x;
    int32_t y0 = center.y;
    int32_t x = radius;
    int32_t y = 0;
    int32_t error = 3 - 2 * x;

    // convert the color to 16 bit
    uint16_t color16 = color.to16bit();

    while (y <= x)
    {
        for (int32_t i = x0 - x; i <= x0 + x; i++)
        {
            int32_t index1 = (y0 + y) * this->config->width + i;
            int32_t index2 = (y0 - y) * this->config->width + i;
            
            if (index1 >= 0 && index1 < this->totalPixels)
            {
                this->frameBuffer[index1] = color16;
            }
            
            if (index2 >= 0 && index2 < this->totalPixels)
            {
                this->frameBuffer[index2] = color16;
            }
        }
        
        for (int32_t i = x0 - y; i <= x0 + y; i++)
        {
            int32_t index1 = (y0 + x) * this->config->width + i;
            int32_t index2 = (y0 - x) * this->config->width + i;

            if (index1 >= 0 && index1 < this->totalPixels)
            {
                this->frameBuffer[index1] = color16;
            }
            
            if (index2 >= 0 && index2 < this->totalPixels)
            {
                this->frameBuffer[index2] = color16;
            }
        }

        y++;

        // Update the error
        if (error > 0) 
        {
            x--;
            error += 4 * (y - x) + 10;
        }
        else
            error += 4 * y + 6;
    }
}

/**
 * @brief Draw a filled circle with a stroke on the display
 * @param center Center point
 * @param radius Radius of the circle
 * @param fillColor color to fill in
 * @param strokeColor color of the stroke
 * @param strokeThickness Thickness of the stroke
 */
void graphics::drawFilledCircleWithStroke(point center, uint32_t radius, color fillColor, color strokeColor, uint32_t strokeThickness)
{
    this->drawFilledCircle(center, radius, fillColor);
    this->drawCircle(center, radius, strokeColor, strokeThickness);
}

/**
 * @private
 * @brief Function to draw the circle using the Bresenham's circle algorithm
 * @param center Center point
 * @param radius Radius of the circle
 * @param color color to draw in
 */
void graphics::drawCircle1(point center, uint32_t radius, color color)
{
    // move Points into local variables
    uint32_t x0 = center.x;
    uint32_t y0 = center.y;
    int32_t x = radius;
    int32_t y = 0;
    int32_t error = 3 - 2 * x;

    // convert the color to 16 bit
    uint16_t color16 = color.to16bit();

    // loop through the radius
    while(x >= y)
    {
        // draw the pixels in the frame buffer
        this->frameBuffer[(x0 + x) + (y0 + y) * this->config->width] = color16;
        this->frameBuffer[(x0 + y) + (y0 + x) * this->config->width] = color16;
        this->frameBuffer[(x0 - y) + (y0 + x) * this->config->width] = color16;
        this->frameBuffer[(x0 - x) + (y0 + y) * this->config->width] = color16;
        this->frameBuffer[(x0 - x) + (y0 - y) * this->config->width] = color16;
        this->frameBuffer[(x0 - y) + (y0 - x) * this->config->width] = color16;
        this->frameBuffer[(x0 + y) + (y0 - x) * this->config->width] = color16;
        this->frameBuffer[(x0 + x) + (y0 - y) * this->config->width] = color16;
        
        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y point
        y++;
    }
}

/**
 * @private
 * @brief Function to draw the circle using a modified Bresenham's circle algorithm
 * @param center Center point
 * @param radius Radius of the circle
 * @param color color to draw in
 * @param thickness Thickness of the circle
 */
void graphics::drawCircle2(point center, uint32_t radius, color color, uint32_t thickness)
{
    uint32_t thickness_mod = thickness >> 1;
    uint32_t x_outer = radius + thickness_mod;
    uint32_t x_inner = radius - thickness_mod;
    uint32_t y = 0;
    uint32_t x0 = center.x;
    uint32_t y0 = center.y;
    int32_t erro = 1 - x_outer;
    int32_t erri = 1 - x_inner;
    uint16_t color16 = color.to16bit();

    while (x_outer >= y)
    {
        this->drawCircleXLine(x0 + x_inner, x0 + x_outer, y0 + y,  color);
        this->drawCircleYLine(x0 + y,  y0 + x_inner, y0 + x_outer, color);
        this->drawCircleXLine(x0 - x_outer, x0 - x_inner, y0 + y,  color);
        this->drawCircleYLine(x0 - y,  y0 + x_inner, y0 + x_outer, color);
        this->drawCircleXLine(x0 - x_outer, x0 - x_inner, y0 - y,  color);
        this->drawCircleYLine(x0 - y,  y0 - x_outer, y0 - x_inner, color);
        this->drawCircleXLine(x0 + x_inner, x0 + x_outer, y0 - y,  color);
        this->drawCircleYLine(x0 + y,  y0 - x_outer, y0 - x_inner, color);

        y++;

        if (erro < 0)
        {
            erro += 2 * y + 1;
        }
        else
        {
            x_outer--;
            erro += 2 * (y - x_outer + 1);
        }

        if (y > x_inner)
        {
            x_inner = y;
        }
        else
        {
            if (erri < 0)
            {
                erri += 2 * y + 1;
            }
            else
            {
                x_inner--;
                erri += 2 * (y - x_inner + 1);
            }
        }
    }
}

/**
 * @private
 * @brief Helper function to draw a horizontal line of a circle
 * @param x1 Start x coordinate
 * @param x2 End x coordinate
 * @param y Y coordinate
 * @param color color of the line
 */
void graphics::drawCircleXLine(uint32_t x1, uint32_t x2, uint32_t y, color color)
{
    while (x1 <= x2) this->setPixel(x1++, y, color.to16bit());
}

/**
 * @private
 * @brief Helper function to draw a vertical line of a circle
 * @param x X coordinate
 * @param y1 Start y coordinate
 * @param y2 End y coordinate
 * @param color color of the line
 */
void graphics::drawCircleYLine(uint32_t x, uint32_t y1, uint32_t y2, color color)
{
    while (y1 <= y2) this->setPixel(x, y1++, color.to16bit());
}