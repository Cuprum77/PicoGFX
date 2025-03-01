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

/**
 * @brief Draw an arc to the display
 * @param center Center point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param color color to draw in
*/
void graphics::drawArc(point center, uint32_t radius, uint32_t start_angle, uint32_t end_angle, color color)
{
    uint32_t imageWidth = config->width;
    uint32_t imageHeight = config->height;

	// Swap angles if start_angle is greater than end_angle
    if (end_angle < start_angle) 
    {
        uint32_t temp = end_angle;
        end_angle = start_angle;
        start_angle = temp;
    }

	// convert the color to 16 bit
    uint16_t color16bit = color.to16bit();

    // loop through the angles
    for (int32_t angle = start_angle; angle < end_angle; angle++) 
    {
		// Get the coordinates of the pixel
        int32_t x = 0;
        int32_t y = 0;
		pcircle(radius, angle, center.x, center.y, &x, &y);

		// avoid overflowing the buffer
        if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight)
            this->frameBuffer[x + y * imageWidth] = color16bit;
    }
}

/**
 * @brief Draw two arcs and fill the gap between them
 * @param center Center point
 * @param innerRadius Radius for the inner most arc
 * @param outerRadius Radius for the outer most arc
 * @param startAngle Angle in degrees for both arcs
 * @param endAngle Angle in degrees for both arcs
 * @color color to draw the arc in
 */
void graphics::drawFilledDualArc(point center, uint32_t innerRadius, uint32_t outerRadius, uint32_t startAngle, uint32_t endAngle, color color)
{
    // Transform angles from [-180, 180] to [0, 360)
    startAngle = inorm(startAngle);
    endAngle = inorm(endAngle);
    startAngle *= 10;
    endAngle *= 10;
    
    if (endAngle < startAngle) endAngle += 3600;

    uint16_t color16 = color.to16bit();

    for (int32_t angleLUT = startAngle; angleLUT <= endAngle; angleLUT++)
    {
        int32_t cosValue = icosd(angleLUT);
        int32_t sinValue = isind(angleLUT);

        for (int32_t radius = innerRadius; radius <= outerRadius; radius++)
        {
            int32_t x = cosValue * radius;
            x >>= COS_MULTIPLIER_BITS;
            x += center.x;

            int32_t y = sinValue * radius;
            y >>= SIN_MULTIPLIER_BITS;
            y += center.y;

            if (x >= 0 && x < config->width && y >= 0 && y < config->height)
                this->frameBuffer[x + y * config->width] = color16;
        }
    }
}