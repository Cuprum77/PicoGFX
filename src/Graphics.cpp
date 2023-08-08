#include "Graphics.hpp"
#include <stdio.h>

/**
 * @brief Construct a new Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
Graphics::Graphics(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->params = params;
    this->totalPixels = params.width * params.height;
}

/**
 * @brief Fill the display with a color
 * @param color Color to fill with
*/
void Graphics::fill(Color color)
{
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();
    // fill the frame buffer
    for (int i = 0; i < this->totalPixels; i++)
        this->frameBuffer[i] = color16;
}

/**
 * @brief Fill the display with a color gradient
 * @param startColor Color to start with
 * @param endColor Color to end with
 * @param start Start Point
 * @param end End Point
 * @note The start and end points are only used to find the direction of the gradient, it will still fill the entire display!
*/
void Graphics::fillGradientCool(Color startColor, Color endColor, Point start, Point end)
{
    // check if the start and end Points are the same
    if(start == end)
        return;

    // set the start to end if end is less than start
    if(end.X() < start.Y() && end.Y() < start.Y())
    {
        Point temp = start;
        start = end;
        end = temp;
    }

    // calculate the length of the line
    unsigned int length = start.Distance(end);

    // calculate the direction of the gradient
    unsigned int deltaX = end.X() - start.X();
    unsigned int deltaY = end.Y() - start.Y();
    unsigned int magnitude = isqrt((deltaX * deltaX) + (deltaY * deltaY));
    float gradX = deltaX / magnitude;
    float gradY = deltaY / magnitude;

    // loop through each pixel in the buffer
    for(int y = 0; y < this->params.height; y++)
    {
        for(int x = 0; x < this->params.width; x++)
        {
            // calculate the position along the gradient direction
            float position = (gradX * x) + (gradY * y);

            // interpolate the color based on the position
            Color color;
            color.r = startColor.r + ((endColor.r - startColor.r) * position / length);
            color.g = startColor.g + ((endColor.g - startColor.g) * position / length);
            color.b = startColor.b + ((endColor.b - startColor.b) * position / length);

            // draw the pixel
            this->frameBuffer[x + y * this->params.width] = color.to16bit();
        }
    }
}

/**
 * @brief Draw a line on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Graphics::drawLine(Point start, Point end, Color color)
{
    // Uses Bresenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // move Points into local variables
    unsigned int x0 = start.X();
    unsigned int y0 = start.Y();
    unsigned int x1 = end.X();
    unsigned int y1 = end.Y();

    // Clamp the x and y Points
    if(x0 > this->params.width) x0 = this->params.width - 1;
    if(y0 > this->params.height) y0 = this->params.height - 1;
    if(x1 > this->params.width) x1 = this->params.width - 1;
    if(y1 > this->params.height) y1 = this->params.height - 1;

    // get the difference between the x and y Points
    int dx = iabs((int)end.X() - (int)start.X());
    int sx = start.X() < end.X() ? 1 : -1;
    int dy = -(int)iabs((int)end.Y() - (int)start.Y());
    int sy = start.Y() < end.Y() ? 1 : -1;
    // calculate the error
    int error = dx + dy;
    
    while(true)
    {
        // set the pixel in the frame buffer
        this->frameBuffer[x0 + y0 * this->params.width] = color.to16bit();

        // if we have reached the end Point, break
        if(x0 == x1 && y0 == y1) break;

        // calculate the error
        int e2 = 2 * error;
        // if the error is greater than the difference in y
        if(e2 >= dy)
        {
            // if we have reached the end Point, break
            if(x0 == x1) break;
            // increment the x Point
            error += dy;
            x0 += sx;
        }
        // if the error is greater than the difference in x
        if(e2 <= dx)
        {
            // if we have reached the end Point, break
            if(y0 == y1) break;
            // increment the y Point
            error += dx;
            y0 += sy;
        }
    }
}

/**
 * @brief Draw a line on the display that expands in thickness
 * @param start Start Point
 * @param end End Point
 * @param startThickness Thickness at the start point
 * @param endThickness Thickness at the end point
 * @param color Color to draw in
*/
void Graphics::drawWedge(Point start, Point end, unsigned int startThickness, unsigned int endThickness, Color color)
{
    // We need to calculate the offset of the line both for the start and end points
    int startThicknessHalf = startThickness >> 1; // Divide by 2
    int endThicknessHalf = endThickness >> 1; // Divide by 2

    // Make sure the thickness is at least 1
    if(startThicknessHalf == 0) startThicknessHalf = 1;
    if(endThicknessHalf == 0) endThicknessHalf = 1;
    
    // Set I to be the inverse of the thickness half to get the offset
    int i = -startThicknessHalf;
    // If however the thickness is 1, we should change the variables so we can still draw the line
    if (startThickness <= 1)
    {
        i = 0;
        startThicknessHalf = 1;
    }

    // Itterate through the start thickness
    for (int i = -startThicknessHalf; i < startThicknessHalf; i++)
    {
        // Find the normal vector at the start point
        int nxA = start.X() - end.X();
        int nyA = start.Y() - end.Y();
        // Offset the normal vector to be at the correct position
        nxA += i;
		nyA += i;

        // Do the exact same thing for the end thickness
        int j = -endThicknessHalf;
        // If however the thickness is 1, we should change the variables so we can still draw the line
        if (endThickness <= 1)
		{
			j = 0;
			endThicknessHalf = 1;
		}
		// Itterate through the end thickness
		for (int j = -endThicknessHalf; j < endThicknessHalf; j++)
		{
            // Find the normal vector at the end point
			int nxB = end.X() - start.X();
			int nyB = end.Y() - start.Y();
			// Offset the normal vector to be at the correct position
			nxB += j;
			nyB += j;

            int startX = start.X() + nxA;
            int startY = start.Y() + nyA;
            int endX = end.X() + nxB;
            int endY = end.Y() + nyB;

            // Verify that these coordinates are within the bounds of the display
            if(startX < 0 || startX > this->params.width || startY < 0 || startY > this->params.height) continue;
            if(endX < 0 || endX > this->params.width || endY < 0 || endY > this->params.height) continue;

			// Draw the line
			this->drawLine(Point(start.X() + nxA, start.Y() + nyA), Point(end.X() + nxB, end.Y() + nyB), color);
		}
    }
}

/**
 * @brief Draw a triangle on the display
 * @param p1 First point
 * @param p2 Second point
 * @param p3 Third point
 * @param color Color to draw in
*/
void Graphics::drawTriangle(Point p1, Point p2, Point p3, Color color)
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
 * @param color Color to draw in
*/
void Graphics::drawFilledTriangle(Point p1, Point p2, Point p3, Color color)
{
    // calculate the bounding box of the triangle
    int minX = imin(imin(p1.x, p2.x), p3.x);
    int maxX = imax(imax(p1.x, p2.x), p3.x);
    int minY = imin(imin(p1.y, p2.y), p3.y);
    int maxY = imax(imax(p1.y, p2.y), p3.y);

    // convert the color to unsigned short
    unsigned short color16 = color.to16bit();

    // iterate over each row within the bounding box
    for (int y = minY; y <= maxY; y++)
    {
		// find the start x by interpolating between p1 and p2
		int divisor = (p2.y - p1.y) == 0 ? 1 : p2.y - p1.y;
		int startX = p1.x + (y - p1.y) * (p2.x - p1.x) / divisor;
		// find the end x by interpolating between p2 and p3
		divisor = (p3.y - p2.y) == 0 ? 1 : p3.y - p2.y;
		int endX = p2.x + (y - p2.y) * (p3.x - p2.x) / divisor;

        // ensure startX <= endX
        if (startX > endX)
        {
            int temp = startX;
            startX = endX;
            endX = temp;
        }

		// clamp the start and end points the screen
        startX = imax(startX, 0);
		endX = imin(endX, this->params.width);

        // fill the pixels between the intersection points
        for (int x = startX; x <= endX; x++)
        {
            this->frameBuffer[x + y * this->params.width] = color16;
        }
    }
}

/**
 * @brief Draw a rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Graphics::drawRectangle(Point start, Point end, Color color)
{
    // draw the rectangle
    this->drawLine({start.X(), start.Y()}, {end.X(), start.Y()}, color);
    this->drawLine({end.X(), start.Y()}, {end.X(), end.Y()}, color);
    this->drawLine({end.X(), end.Y()}, {start.X(), end.Y()}, color);
    this->drawLine({start.X(), end.Y()}, {start.X(), start.Y()}, color);
}

/**
 * @brief Draw a rectangle on the display
 * @param rect Rectangle to draw
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Graphics::drawRectangle(Rect rect, Color color)
{
    // draw the rectangle
    this->drawRectangle(rect.X(), rect.Y(), color);
}

/**
 * @brief Draw a center aligned rectangle on the display
 * @param center Center Point
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Graphics::drawRectangle(Point center, unsigned int width, unsigned int height, Color color)
{
    // calculate the start and end Points
    Point start = {center.X() - (width / 2), center.Y() - (height / 2)};
    Point end = {center.X() + (width / 2), center.Y() + (height / 2)};

    // draw the rectangle
    this->drawRectangle(start, end, color);
}

/**
 * @brief Draw a filled rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Graphics::drawFilledRectangle(Point start, Point end, Color color)
{
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();

    // calculate the size of the rectangle
    unsigned int width = end.X() - start.X();
    unsigned int height = end.Y() - start.Y();

    // loop through the height
    for (int i = 0; i < height; i++)
    {
        // loop through the width
        for (int j = 0; j < width; j++)
        {
            // write the pixel
            this->frameBuffer[(start.X() + j) + (start.Y() + i) * this->params.width] = color16;
        }
    }
}

void Graphics::drawPolygon(Point* points, size_t numberOfPoints, Color color)
{
    // Make sure theres at least 3 points
    if (numberOfPoints < 3) return;

    // Draw the lines between the points
    for (int i = 0; i < numberOfPoints - 1; i++)
		// Draw the line
		this->drawLine(points[i], points[i + 1], color);

    // Draw the last line
	this->drawLine(points[numberOfPoints - 1], points[0], color);
}

void Graphics::drawFilledPolygon(Point* points, size_t numberOfPoints, Color color)
{
    // Make sure theres at least 3 points
    if (numberOfPoints < 3) return;

    // Set the min and max values to the absolute max and min
    int minX = 0xffffffff;
    int maxX = 0;
    int minY = 0xffffffff;
    int maxY = 0;

    // Get the unsigned short version of the color
    unsigned short color16 = color.to16bit();

    // Calculate the bounding box of the polygon by first finding the min and max x and y values
    for (int i = 0; i < numberOfPoints; i++)
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
    for (int y = minY; y <= maxY; y++) {
        int xStart = maxX;
        int xEnd = minX;

        // Iterate over the edges of the polygon, finding intersections with the scanline
        for (int i = 0; i < numberOfPoints; i++) {
            int nextIndex = i + 1;
            // Manually handle the wrap-around condition
            if (nextIndex == numberOfPoints) nextIndex = 0;

            int deltaY = points[nextIndex].y - points[i].y;
            // Check if the scanline intersects with this edge
            if ((points[i].y <= y && points[nextIndex].y > y) || (points[nextIndex].y <= y && points[i].y > y))
            {
                // Compute the x coordinate of the intersection, avoiding division by zero
                int x = points[i].x;
                if (deltaY != 0) {
                    x += (y - points[i].y) * (points[nextIndex].x - points[i].x) / deltaY;
                }
                if (x < xStart) xStart = x;
                if (x > xEnd) xEnd = x;
            }
        }

        // Fill in the pixels between the start and end intersections
        for (int x = xStart; x < xEnd; x++)
            this->frameBuffer[x + y * this->params.width] = color16;
    }
}

/**
 * @brief Draw a circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Graphics::drawCircle(Point center, unsigned int radius, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.X();
    int y0 = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // convert the color to 16 bit
    unsigned short color16 = color.to16bit();

    // loop through the radius
    while(x >= y)
    {
        // draw the pixels in the frame buffer
        this->frameBuffer[(x0 + x) + (y0 + y) * this->params.width] = color16;
        this->frameBuffer[(x0 + y) + (y0 + x) * this->params.width] = color16;
        this->frameBuffer[(x0 - y) + (y0 + x) * this->params.width] = color16;
        this->frameBuffer[(x0 - x) + (y0 + y) * this->params.width] = color16;
        this->frameBuffer[(x0 - x) + (y0 - y) * this->params.width] = color16;
        this->frameBuffer[(x0 - y) + (y0 - x) * this->params.width] = color16;
        this->frameBuffer[(x0 + y) + (y0 - x) * this->params.width] = color16;
        this->frameBuffer[(x0 + x) + (y0 - y) * this->params.width] = color16;
        
        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y Point
        y++;
    }
}

/**
 * @brief Draw a filled circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Graphics::drawFilledCircle(Point center, unsigned int radius, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.X();
    int y0 = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // loop through the radius
    while(x >= y)
    {
        // draw the pixel
        this->drawLine({x0 + x, y0 + y}, {x0 - x, y0 + y}, color);
        this->drawLine({x0 + y, y0 + x}, {x0 - y, y0 + x}, color);
        this->drawLine({x0 + x, y0 - y}, {x0 - x, y0 - y}, color);
        this->drawLine({x0 + y, y0 - x}, {x0 - y, y0 - x}, color);

        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y Point
        y++;
    }
}

/**
 * @brief Draw an arc to the display
 * @param center Center point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param color Color to draw in
*/
void Graphics::drawArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, Color color)
{
    unsigned int imageWidth = params.width;
    unsigned int imageHeight = params.height;

	// Swap angles if start_angle is greater than end_angle
    if (end_angle < start_angle) 
    {
        unsigned int temp = end_angle;
        end_angle = start_angle;
        start_angle = temp;
    }

	// clamp the input variables to be between 0 and 3600
	start_angle = imin(start_angle, NUMBER_OF_ANGLES);
	end_angle = imin(end_angle, NUMBER_OF_ANGLES);

	// convert the color to 16 bit
    unsigned short color16bit = color.to16bit();

    // loop through the angles
    for (int angle = start_angle; angle < end_angle; angle++) 
    {
		// Get the coordinates of the pixel
        unsigned int x = 0;
        unsigned int y = 0;
		pointOnCircle(radius, angle, center.x, center.y, &x, &y);

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
 * @color Color to draw the arc in
 */
void Graphics::drawFilledDualArc(Point center, int innerRadius, int outerRadius, int startAngle, int endAngle, Color color)
{
    // Transform angles from [-180, 180] to [0, 360)
    if (startAngle < 0) startAngle += 360;
    if (endAngle < 0) endAngle += 360;
    if (endAngle < startAngle) endAngle += 360;

    unsigned short color16 = color.to16bit();

    for (int angleLUT = startAngle * 10; angleLUT <= endAngle * 10; angleLUT++)
    {
        // Handle the angle wrap at 3600 manually
        int wrappedAngleLUT = angleLUT;
        if (wrappedAngleLUT >= 3600) wrappedAngleLUT -= 3600;

        int cosValue = cosTable[wrappedAngleLUT];
        int sinValue = sinTable[wrappedAngleLUT];

        for (int radius = innerRadius; radius <= outerRadius; radius++)
        {
            int x = cosValue * radius;
            x >>= FIXED_POINT_SCALE_BITS;
            x += center.x;

            int y = sinValue * radius;
            y >>= FIXED_POINT_SCALE_BITS;
            y += center.y;

            if (x >= 0 && x < params.width && y >= 0 && y < params.height)
                this->frameBuffer[x + y * params.width] = color16;
        }
    }
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Graphics::drawBitmap(const unsigned char* bitmap, unsigned int width, unsigned int height)
{
    this->drawBitmap((const unsigned short*)bitmap, width, height);
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Graphics::drawBitmap(const unsigned short* bitmap, unsigned int width, unsigned int height)
{
    // write the entire bitmap directly to the framebuffer using the setPixel function
    for(int i = 0; i < width * height; i++)
    {
        // write the pixel
        this->frameBuffer[i] = bitmap[i];
    }
}

/**
 * @brief Apply a simple blur to the display to combat pixelation
*/
void Graphics::antiAliasingFilter(void)
{
    // Helper function to get the color difference between two pixels
    #define COLOR_DIFF(pixel1, pixel2) ( \
        iabs(((pixel1 & 0xF800) >> 8) - ((pixel2 & 0xF800) >> 8)) + \
        iabs(((pixel1 & 0x07E0) >> 3) - ((pixel2 & 0x07E0) >> 3)) + \
        iabs(((pixel1 & 0x001F) << 3) - ((pixel2 & 0x001F) << 3)) \
    )

    // Loop through all the pixels in the framebuffer
    for (int y = 1; y < (this->params.height - 1); y++)
    {
        for (int x = 1; x < (this->params.width - 1); x++)
        {
            // Get the index of the current pixel
            int pixelIndex = y * this->params.width + x;

            // Get the surrounding pixels
            unsigned short leftPixel = this->frameBuffer[pixelIndex - 1];
            unsigned short rightPixel = this->frameBuffer[pixelIndex + 1];
            unsigned short upPixel = this->frameBuffer[pixelIndex - this->params.width];
            unsigned short downPixel = this->frameBuffer[pixelIndex + this->params.width];            

            // Calculate the  difference between the left and right pixel and the up and down pixel
            int horizontalDiff = COLOR_DIFF(leftPixel, rightPixel);
            int verticalDiff = COLOR_DIFF(upPixel, downPixel);

            // If the difference is large enough, average the colors
            if (horizontalDiff > 16 && verticalDiff > 16)
            {
                // Get the sum of the colors
                int sumR = ((leftPixel & 0xF800) + (rightPixel & 0xF800) + (upPixel & 0xF800) + (downPixel & 0xF800)) >> 8;
                int sumG = ((leftPixel & 0x07E0) + (rightPixel & 0x07E0) + (upPixel & 0x07E0) + (downPixel & 0x07E0)) >> 3;
                int sumB = ((leftPixel & 0x001F) + (rightPixel & 0x001F) + (upPixel & 0x001F) + (downPixel & 0x001F)) << 3;
                // Average the colors
                unsigned short avgR = (sumR >> 2) & 0xF8;
                unsigned short avgG = (sumG >> 2) & 0xFC;
                unsigned short avgB = (sumB >> 2) >> 3;
                // Write the averaged color to the framebuffer
                this->frameBuffer[pixelIndex] = (avgR << 8) | (avgG << 3) | avgB;
            }
        }
    }
}