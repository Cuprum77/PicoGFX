#include "graphics.h"

/**
 * @brief Draw a rectangle on the display
 * @param start Start point
 * @param end End point
 * @param color color to draw in
 * @param thickness Thickness of the line
*/
void graphics::drawRectangle(point start, point end, color color)
{
    // limit the start and end points to the display
    start.x = imax(0, imin(start.x, this->display_ptr->getWidth() - 1));
    start.y = imax(0, imin(start.y, this->display_ptr->getHeight() - 1));
    end.x = imax(0, imin(end.x, this->display_ptr->getWidth() - 1));
    end.y = imax(0, imin(end.y, this->display_ptr->getHeight() - 1));

    // draw the rectangle
    this->drawLine({start.x, start.y}, {end.x, start.y}, color);
    this->drawLine({end.x, start.y}, {end.x, end.y}, color);
    this->drawLine({end.x, end.y}, {start.x, end.y}, color);
    this->drawLine({start.x, end.y}, {start.x, start.y}, color);
}

/**
 * @brief Draw a rectangle on the display
 * @param rect Rectangle to draw
 * @param color color to draw in
 * @param thickness Thickness of the line
*/
void graphics::drawRectangle(rect rect, color color)
{
    // draw the rectangle
    this->drawRectangle(rect.x(), rect.y(), color);
}

/**
 * @brief Draw a center aligned rectangle on the display
 * @param center Center point
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @param color color to draw in
 * @param thickness Thickness of the line
*/
void graphics::drawRectangle(point center, uint32_t width, uint32_t height, color color)
{
    // calculate the start and end Points
    point start = {center.x - (width / 2), center.y - (height / 2)};
    point end = {center.x + (width / 2), center.y + (height / 2)};

    // draw the rectangle
    this->drawRectangle(start, end, color);
}

/**
 * @brief Draw a filled rectangle on the display
 * @param start Start point
 * @param end End point
 * @param color color to draw in
*/
void graphics::drawFilledRectangle(point start, point end, color color)
{
    color_t colorWord = color.toWord();

    // calculate the size of the rectangle
    uint32_t width = end.x - start.x;
    uint32_t height = end.y - start.y;

    // loop through the height
    for (int32_t i = 0; i < height; i++)
    {
        // loop through the width
        for (int32_t j = 0; j < width; j++)
        {
            // write the pixel
            this->frameBuffer[(start.x + j) + (start.y + i) * this->display_ptr->getWidth()] = colorWord;
        }
    }
}

void graphics::drawPolygon(point *points, size_t numberOfPoints, color color)
{
    // Make sure theres at least 3 points
    if (numberOfPoints < 3) return;

    // Draw the lines between the points
    for (int32_t i = 0; i < numberOfPoints - 1; i++)
		// Draw the line
		this->drawLine(points[i], points[i + 1], color);

    // Draw the last line
	this->drawLine(points[numberOfPoints - 1], points[0], color);
}

void graphics::drawFilledPolygon(point *points, size_t numberOfPoints, color color)
{
    // Make sure theres at least 3 points
    if (numberOfPoints < 3) return;

    // Set the min and max values to the absolute max and min
    int32_t minX = 0xffffffff;
    int32_t maxX = 0;
    int32_t minY = 0xffffffff;
    int32_t maxY = 0;
    color_t colorWord = color.toWord();

    // Calculate the bounding box of the polygon by first finding the min and max x and y values
    for (int32_t i = 0; i < numberOfPoints; i++)
    {
        // Check if the x value is less than the min x value
        if(points[i].x < minX) minX = points[i].x;
        // Check if the x value is greater than the max x value
        if(points[i].x > maxX) maxX = points[i].x;
        // Check if the y value is less than the min y value
        if(points[i].y < minY) minY = points[i].y;
        // Check if the y value is greater than the max y value
		if(points[i].y > maxY) maxY = points[i].y;
    }

    // Implement a scanline algorithm to fill in the polygon
    for (int32_t y = minY; y <= maxY; y++) {
        int32_t xStart = maxX;
        int32_t xEnd = minX;

        // Iterate over the edges of the polygon, finding intersections with the scanline
        for (int32_t i = 0; i < numberOfPoints; i++) {
            int32_t nextIndex = i + 1;
            // Manually handle the wrap-around condition
            if (nextIndex == numberOfPoints) nextIndex = 0;

            int32_t deltaY = points[nextIndex].y - points[i].y;
            // Check if the scanline intersects with this edge
            if ((points[i].y <= y && points[nextIndex].y > y) || (points[nextIndex].y <= y && points[i].y > y))
            {
                // Compute the x coordinate of the intersection, avoiding division by zero
                int32_t x = points[i].x;
                if (deltaY != 0) {
                    x += (y - points[i].y) * (points[nextIndex].x - points[i].x) / deltaY;
                }
                if (x < xStart) xStart = x;
                if (x > xEnd) xEnd = x;
            }
        }

        // Fill in the pixels between the start and end intersections
        for (int32_t x = xStart; x < xEnd; x++)
        {
            // Verify that index is within the bounds of the framebuffer
            this->frameBuffer[x + y * this->display_ptr->getWidth()] = colorWord;
        }
    }
}