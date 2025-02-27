#include "graphics.hpp"

/**
 * @brief Draw a line on the display
 * @param start Start point
 * @param end End point
 * @param color color to draw in
*/
void graphics::drawLine(point start, point end, color color)
{
    // Uses an optimized Bresenham's line algorithm
    // http://members.chello.at/~easyfilter/bresenham.html

    // Find the delta x and start x
    int32_t dx = iabs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    // Find the delta y and start y
    int32_t dy = -iabs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    // Calculate the error
    int32_t err = dx + dy, e2;
    // Get the uint16_t color
    uint16_t color16 = color.to16bit();

    // Loop until we break
    for (;;)
    {
        // Set the pixel at the current position
        this->frameBuffer[start.x + start.y * this->config->width] = color16;
        // Check if we are at the end
        if (start.x == end.x && start.y == end.y) break;
        // Calculate the new error
        e2 = 2 * err;
        // Check if we should move in the x direction
        if (e2 >= dy) err += dy; start.x += sx;
        // Check if we should move in the y direction
        if (e2 <= dx) err += dx; start.y += sy;
    }
}

/**
 * @brief Draw a line on the display with anti-aliasing
 * @param start Start point
 * @param end End point
 * @param color color to draw in
*/
void graphics::drawLineAntiAliased(point start, point end, color color)
{
    // Uses an optimized Bresenham's line algorithm
    // http://members.chello.at/~easyfilter/bresenham.html

    // Find the delta x and start x
    int32_t dx = iabs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    // Find the delta y and start y
    int32_t dy = -iabs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    // Calculate the error
    int32_t err = dx - dy;
    int32_t e2, x2;
    int32_t ed = dx + dy == 0 ? 1 : isqrt(dx * dx + dy * dy);
    // Precalculate the inverse of ed for faster calculations
    int32_t edInv = (FIXED_POINT_SCALE + (ed / 2)) / ed;
    // Get the uint16_t color
    uint16_t color16 = color.to16bit();

    // Loop until we break
    for (;;)
    {
        // Set the pixel at the current position
        uint8_t alpha = 255 * iabs(err - dx + dy) * edInv;
        printf("%d ", alpha);
        alpha >>= FIXED_POINT_SCALE_BITS;
        printf("%d\n", alpha);
        this->setPixelBlend(start.x, start.y, color16, alpha);
        // Calculate the new error
        e2 = err; x2 = start.x;

        // Handle the X direction
        if (2 * e2 >= -dx)
        {
            // Break if we are at the end
            if (start.x == end.x) break;
            // Set the pixel at the next position
            if (e2 + dy < ed)
            {
                // Handle the anti-aliasing
                alpha = 255 * (e2 + dy) * edInv;
                alpha >>= FIXED_POINT_SCALE_BITS;
                this->setPixelBlend(start.x, start.y + sy, color16, alpha);
            }
            // Update the error
            err -= dy; start.x += sx;
        }
        // Handle the Y direction
        if (2 * e2 <= dy)
        {
			// Break if we are at the end
			if (start.y == end.y) break;
			// Set the pixel at the next position
            if (dx - e2 < ed)
            {
                // Handle the anti-aliasing
                alpha = 255 * (dx - e2) * edInv;
                alpha >>= FIXED_POINT_SCALE_BITS;
                this->setPixelBlend(x2 + sx, start.y, color16, alpha);
            }
            // Update the error
            err += dx; start.y += sy;
		}
    }
}

/**
 * @brief Draw a thick line on the display with anti-aliasing
 * @param start Start point
 * @param end End point
 * @param thickness Thickness of the line, value must be greater than 1
 * @param color color to draw in
*/
void graphics::drawLineThickAntiAliased(point start, point end, uint32_t thickness, color color)
{
    // Uses an optimized Bresenham's line algorithm
    // http://members.chello.at/~easyfilter/bresenham.html

    // Find the delta x and start x
    int32_t dx = iabs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    // Find the delta y and start y
    int32_t dy = -iabs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    // Calculate the error
    int32_t err = dx - dy;
    int32_t e2, x2, y2;
    int32_t ed = dx + dy == 0 ? 1 : isqrt(dx * dx + dy * dy);
    // Get the uint16_t color
    uint16_t color16 = color.to16bit();
}