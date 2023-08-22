#include "Graphics.hpp"
#include <stdio.h>

/**
 * @brief Construct a new Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
Graphics::Graphics(unsigned short* frameBuffer, display_config_t* config)
{
    this->frameBuffer = frameBuffer;
    this->config = config;
    this->totalPixels = config->width * config->height;
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
 * @brief Draw a line on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Graphics::drawLine(Point start, Point end, Color color)
{
    // Uses an optimized Bresenham's line algorithm
    // http://members.chello.at/~easyfilter/bresenham.html

    // Find the delta x and start x
    int dx = iabs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    // Find the delta y and start y
    int dy = -iabs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    // Calculate the error
    int err = dx + dy, e2;
    // Get the unsigned short color
    unsigned short color16 = color.to16bit();

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
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Graphics::drawLineAntiAliased(Point start, Point end, Color color)
{
    // Uses an optimized Bresenham's line algorithm
    // http://members.chello.at/~easyfilter/bresenham.html

    // Find the delta x and start x
    int dx = iabs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    // Find the delta y and start y
    int dy = -iabs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    // Calculate the error
    int err = dx - dy;
    int e2, x2;
    int ed = dx + dy == 0 ? 1 : isqrt(dx * dx + dy * dy);
    // Precalculate the inverse of ed for faster calculations
    int edInv = (FIXED_POINT_SCALE + (ed / 2)) / ed;
    // Get the unsigned short color
    unsigned short color16 = color.to16bit();

    // Loop until we break
    for (;;)
    {
        // Set the pixel at the current position
        unsigned char alpha = 255 * iabs(err - dx + dy) * edInv;
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
 * @param start Start Point
 * @param end End Point
 * @param thickness Thickness of the line, value must be greater than 1
 * @param color Color to draw in
*/
void Graphics::drawLineThickAntiAliased(Point start, Point end, int thickness, Color color)
{
    // Uses an optimized Bresenham's line algorithm
    // http://members.chello.at/~easyfilter/bresenham.html

    // Find the delta x and start x
    int dx = iabs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    // Find the delta y and start y
    int dy = -iabs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    // Calculate the error
    int err = dx - dy;
    int e2, x2, y2;
    int ed = dx + dy == 0 ? 1 : isqrt(dx * dx + dy * dy);
    // Get the unsigned short color
    unsigned short color16 = color.to16bit();
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
		endX = imin(endX, this->config->width);

        // fill the pixels between the intersection points
        for (int x = startX; x <= endX; x++)
        {
            this->frameBuffer[x + y * this->config->width] = color16;
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
    this->drawLine({start.x, start.y}, {end.x, start.y}, color);
    this->drawLine({end.x, start.y}, {end.x, end.y}, color);
    this->drawLine({end.x, end.y}, {start.x, end.y}, color);
    this->drawLine({start.x, end.y}, {start.x, start.y}, color);
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
    Point start = {center.x - (width / 2), center.y - (height / 2)};
    Point end = {center.x + (width / 2), center.y + (height / 2)};

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
    unsigned int width = end.x - start.x;
    unsigned int height = end.y - start.y;

    // loop through the height
    for (int i = 0; i < height; i++)
    {
        // loop through the width
        for (int j = 0; j < width; j++)
        {
            // write the pixel
            this->frameBuffer[(start.x + j) + (start.y + i) * this->config->width] = color16;
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
        {
            // Verify that index is within the bounds of the framebuffer
            this->frameBuffer[x + y * this->config->width] = color16;
        }
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
    int x0 = center.x;
    int y0 = center.y;
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // convert the color to 16 bit
    unsigned short color16 = color.to16bit();

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
    // Uses a modified Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.x;
    int y0 = center.y;
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // convert the color to 16 bit
    unsigned short color16 = color.to16bit();

    while (y <= x)
    {
        for (int i = x0 - x; i <= x0 + x; i++)
        {
            int index1 = (y0 + y) * this->config->width + i;
            int index2 = (y0 - y) * this->config->width + i;
            
            if (index1 >= 0 && index1 < this->totalPixels)
            {
                this->frameBuffer[index1] = color16;
            }
            
            if (index2 >= 0 && index2 < this->totalPixels)
            {
                this->frameBuffer[index2] = color16;
            }
        }
        
        for (int i = x0 - y; i <= x0 + y; i++)
        {
            int index1 = (y0 + x) * this->config->width + i;
            int index2 = (y0 - x) * this->config->width + i;

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
 * @brief Draw an arc to the display
 * @param center Center point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param color Color to draw in
*/
void Graphics::drawArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, Color color)
{
    unsigned int imageWidth = config->width;
    unsigned int imageHeight = config->height;

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

            if (x >= 0 && x < config->width && y >= 0 && y < config->height)
                this->frameBuffer[x + y * config->width] = color16;
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
    for (int y = 1; y < (this->config->height - 1); y++)
    {
        for (int x = 1; x < (this->config->width - 1); x++)
        {
            // Get the index of the current pixel
            int pixelIndex = y * this->config->width + x;

            // Get the surrounding pixels
            unsigned short leftPixel = this->frameBuffer[pixelIndex - 1];
            unsigned short rightPixel = this->frameBuffer[pixelIndex + 1];
            unsigned short upPixel = this->frameBuffer[pixelIndex - this->config->width];
            unsigned short downPixel = this->frameBuffer[pixelIndex + this->config->width];            

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

//   rrrrrggggggbbbbb
#define RB_MASK      63519  // 0b1111100000011111        --> hex :F81F
#define G_MASK        2016  // 0b0000011111100000        --> hex :07E0
#define RB_MUL_MASK 2032608 // 0b111110000001111100000   --> hex :1F03E0
#define G_MUL_MASK   64512  // 0b000001111110000000000   --> hex :FC00

/**
 * @private
 * @brief Helper function to blend the pixel color with the background for anti-aliasing
 * @param x X coordinate of the pixel to blend
 * @param y Y coordinate of the pixel to blend
 * @param color Color of the pixel to blend
 * @param alpha Alpha value of the pixel to blend, 0 is transparent, 255 is opaque
 * @note Sets the pixel at the given index to the blended color
 */
void Graphics::setPixelBlend(int x, int y, unsigned short color, unsigned char alpha)
{
    // Taken from this stackoverflow answer https://stackoverflow.com/questions/72456587/implement-a-function-that-blends-two-colors-encoded-with-rgb565-using-alpha-blen

    // Get the buffer index
    int index = x + y * this->config->width;

    // Reduce the alpha from [0,255] to [0,31] 
    alpha = alpha >> 0x3;
    // Create the inverse alpha value and call it beta
    unsigned char beta = 0x20 - alpha;

    // Get the background color
    unsigned short background = this->frameBuffer[index];

    // Apply the alpha blending formula
    unsigned short result = (unsigned short)((((alpha * (unsigned int)(color & RB_MASK) + beta * (unsigned int)(background & RB_MASK)) & RB_MUL_MASK)
        | ((alpha * (color & G_MASK) + beta * (background & G_MASK)) & G_MUL_MASK)) >> 0x5);

    // Apply the blended color to the framebuffer
    this->frameBuffer[index] = result;
}