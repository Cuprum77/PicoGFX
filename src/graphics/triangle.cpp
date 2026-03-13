#include "graphics.h"

/**
 * @brief Draw a triangle on the display
 * @param p1 First point
 * @param p2 Second point
 * @param p3 Third point
 * @param color color to draw in
*/
void graphics::drawTriangle(point p1, point p2, point p3, color color)
{
	// Draw the three lines of the triangle
	this->drawLine(p1, p2, color);
	this->drawLine(p2, p3, color);
	this->drawLine(p3, p1, color);
}

/**
 * @brief Draw a filled triangle on the display
 * @param p1 First point
 * @param p2 Second point
 * @param p3 Third point
 * @param color color to draw in
*/
void graphics::drawFilledTriangle(point p1, point p2, point p3, color color)
{
    // calculate the bounding box of the triangle
    int32_t minX = imin(imin(p1.x, p2.x), p3.x);
    int32_t maxX = imax(imax(p1.x, p2.x), p3.x);
    int32_t minY = imin(imin(p1.y, p2.y), p3.y);
    int32_t maxY = imax(imax(p1.y, p2.y), p3.y);

    // convert the color to uint16_t
    uint16_t color16 = color.toWord();

    // iterate over each row within the bounding box
    for (int32_t y = minY; y <= maxY; y++)
    {
		// find the start x by interpolating between p1 and p2
		int32_t divisor = (p2.y - p1.y) == 0 ? 1 : p2.y - p1.y;
		int32_t startX = p1.x + (y - p1.y) * (p2.x - p1.x) / divisor;
		// find the end x by interpolating between p2 and p3
		divisor = (p3.y - p2.y) == 0 ? 1 : p3.y - p2.y;
		int32_t endX = p2.x + (y - p2.y) * (p3.x - p2.x) / divisor;

        // ensure startX <= endX
        if (startX > endX)
        {
            int32_t temp = startX;
            startX = endX;
            endX = temp;
        }

		// clamp the start and end points the screen
        startX = imax(startX, 0);
		endX = imin(endX, this->display_ptr->getWidth());

        // fill the pixels between the intersection points
        for (int32_t x = startX; x <= endX; x++)
        {
            this->frameBuffer[x + y * this->display_ptr->getWidth()] = color16;
        }
    }
}