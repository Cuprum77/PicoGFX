#include "Display.hpp"

/**
 * @brief Fill the display with a gradient
 * @param startColor Color to start with
 * @param endColor Color to end with
 * @param start Start Point
 * @param end End Point
 * @param steps Number of steps to take
*/
void Display::fillGradient(Color startColor, Color endColor, Point start, Point end, uint steps)
{
    // make sure steps is always at least 1
    if(steps < 1) steps = 1;
    // calculate the length of the line
    uint length = start.Distance(end);
    // make an array of colors
    this->interpolate(this->frameBufferRow, length, startColor, endColor, steps);

    // check if its purely vertical or horizontal to speed it up
    
    // if its vertical
    if(start.X() == end.X())
    {
        for(int y = 0; y < this->params.height; y++)
        {
            // draw the line
            this->drawLine({start.X(), y}, {end.X(), y}, this->frameBufferRow[y]);
        }
    }

    // if its horizontal
    if(start.Y() == end.Y())
    {
        for(int x = 0; x < this->params.width; x++)
        {
            // draw the line
            this->drawLine({x, start.Y()}, {x, end.Y()}, this->frameBufferRow[x]);
        }
    }

    // if its diagonal
    if(start.X() != end.X() && start.Y() != end.Y())
    {
        // calculate the slope
        float slope = (float)(end.Y() - start.Y()) / (float)(end.X() - start.X());
        // calculate the y intercept
        float yIntercept = start.Y() - (slope * start.X());

        // loop through the x values
        for(int x = 0; x < this->params.width; x++)
        {
            // calculate the y value
            int y = (slope * x) + yIntercept;
            // draw the line
            this->drawLine({x, y}, {x, y}, this->frameBufferRow[x]);
        }
    }
}

/**
 * @brief Draw a line on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Display::drawLine(Point start, Point end, Color color)
{
    // Uses Bresenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // move Points into local variables
    uint x0 = start.X();
    uint y0 = start.Y();
    uint x1 = end.X();
    uint y1 = end.Y();

    // get the difference between the x and y Points
    int dx = abs((int)end.X() - (int)start.X());
    int sx = start.X() < end.X() ? 1 : -1;
    int dy = -abs((int)end.Y() - (int)start.Y());
    int sy = start.Y() < end.Y() ? 1 : -1;
    // calculate the error
    int error = dx + dy;
    
    while(true)
    {
        // draw the pixel
        this->drawPixel({x0, y0}, color);

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
 * @brief Draw a rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Display::drawRectangle(Point start, Point end, Color color)
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
void Display::drawRectangle(Rectangle rect, Color color)
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
void Display::drawRectangle(Point center, uint width, uint height, Color color)
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
void Display::drawFilledRectangle(Point start, Point end, Color color)
{
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();

    // calculate the size of the rectangle
    uint width = end.X() - start.X();
    uint height = end.Y() - start.Y();
    
    this->setCursor({0, 0});
    
    // loop through the height
    for(int i = 0; i < height; i++)
    {
        // move the cursor to the next row
        this->setCursor({start.X(), start.Y() + i});

        // loop through the width
        for(int j = 0; j < width; j++)
        {
            // write the pixel
            this->writePixels(&color16, sizeof(color16));
        }
    }
}

/**
 * @brief Draw a circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Display::drawCircle(Point center, uint radius, Color color)
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
        this->drawPixel({x0 + x, y0 + y}, color);
        this->drawPixel({x0 + y, y0 + x}, color);
        this->drawPixel({x0 - y, y0 + x}, color);
        this->drawPixel({x0 - x, y0 + y}, color);
        this->drawPixel({x0 - x, y0 - y}, color);
        this->drawPixel({x0 - y, y0 - x}, color);
        this->drawPixel({x0 + y, y0 - x}, color);
        this->drawPixel({x0 + x, y0 - y}, color);

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
void Display::drawFilledCircle(Point center, uint radius, Color color)
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
 * @param center Center Point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param color Color to draw in
*/
void Display::drawArc(Point center, uint radius, uint start_angle, uint end_angle, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // lock the angles to 0 - 360
    uint _start_angle = start_angle % 360;
    uint _end_angle = end_angle % 360;

    // if the start angle is greater than the end angle
    if(_start_angle > _end_angle)
    {
        // swap the angles
        _start_angle ^= _end_angle;
        _end_angle ^= _start_angle;
        _start_angle ^= _end_angle;
    }

    // calculate the start and end points
    Point start = {(unsigned int)(center.X() + radius * cos(_start_angle * M_PI / 180)), (unsigned int)(center.Y() + radius * sin(_start_angle * M_PI / 180))};
    Point end = {(unsigned int)(center.X() + radius * cos(_end_angle * M_PI / 180)), (unsigned int)(center.Y() + radius * sin(_end_angle * M_PI / 180))};

    // move Points into local variables
    int xc = center.X();
    int yc = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // loop through the radius
    while (x <= y) {
        // Plot the points within the desired arc range
        if ((x >= _start_angle && x <= _end_angle) || (y >= _start_angle && y <= _end_angle)) {
            printf("(%d, %d)\n", xc + x, yc + y);
            printf("(%d, %d)\n", xc + y, yc + x);
            printf("(%d, %d)\n", xc - x, yc + y);
            printf("(%d, %d)\n", xc - y, yc + x);
            printf("(%d, %d)\n", xc + x, yc - y);
            printf("(%d, %d)\n", xc + y, yc - x);
            printf("(%d, %d)\n", xc - x, yc - y);
            printf("(%d, %d)\n", xc - y, yc - x);
        }
  
        if (error < 0)
            error += (4 * x) + 6;
        else {
            error += (4 * (x - y)) + 10;
            y--;
        }
  
        x++;
    }
}

/**
 * @brief Draw a filled arc to the display
 * @param center Center Point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param outer_radius External radius of the arc
 * @param inner_radius Internal radius of the arc
 * @param color Color to draw in
 * @note This will fill the void between the two radii
*/
void Display::drawFilledArc(Point center, uint radius, uint start_angle, uint end_angle, uint outer_radius, uint inner_radius, Color color)
{
    
}


/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Display::drawBitmap(const uchar* bitmap, uint width, uint height)
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
void Display::drawBitmap(const unsigned short* bitmap, uint width, uint height)
{
    // get the cursor location
    Point location = this->getCursor();

    // write the bitmap
    int offset = 0;
    for(int y = 0; y < height; y++)
    {
        this->setCursor(Point(0 + location.X(), y + location.Y()));
        // write the row of pixels, we need to multiply the width by 2 because we are using 16 bit colors
        this->writePixels(&bitmap[offset], width * 2);
        // increment the offset
        offset += width;
    }
}

/**
 * @private
 * @brief Interpolate between two colors and fill a provided array
 * @param color Array to fill
 * @param len Length of the array
 * @param start Starting color
 * @param end Ending color
 * @param steps Number of steps to interpolate
*/
void Display::interpolate(ushort *color, size_t len, Color start, Color end, uint steps)
{
    if(len > 0)
    {
        // calculate the step size
        float step_size = 1.0 / (float)steps;

        // calculate the step size for each color
        float r_step = (end.r - start.r) * step_size;
        float g_step = (end.g - start.g) * step_size;
        float b_step = (end.b - start.b) * step_size;

        // loop through the array
        for(size_t i = 0; i < len; i++)
        {
            // calculate the color
            ushort r = start.r + (r_step * i);
            ushort g = start.g + (g_step * i);
            ushort b = start.b + (b_step * i);

            // set the color
            color[i] = (r << 11) | (g << 5) | b;
        }
    }
}