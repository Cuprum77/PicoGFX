#include "Graphics.hpp"

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
    unsigned int magnitude = sqrt((deltaX * deltaX) + (deltaY * deltaY));
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
 * @param thickness Thickness of the line
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

    // get the difference between the x and y Points
    int dx = abs((int)end.X() - (int)start.X());
    int sx = start.X() < end.X() ? 1 : -1;
    int dy = -abs((int)end.Y() - (int)start.Y());
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
void Graphics::drawRectangle(Rectangle rect, Color color)
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
    for(int i = 0; i < height; i++)
    {
        // loop through the width
        for(int j = 0; j < width; j++)
        {
            // write the pixel
            this->frameBuffer[(start.X() + j) + (start.Y() + i) * this->params.width] = color16;
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
 * @param center Center Point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param color Color to draw in
*/
void Graphics::drawArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // lock the angles to 0 - 360
    unsigned int _start_angle = start_angle % 360;
    unsigned int _end_angle = end_angle % 360;

    // if the start angle is greater than the end angle
    if(_start_angle > _end_angle)
    {
        // swap the angles
        _start_angle ^= _end_angle;
        _end_angle ^= _start_angle;
        _start_angle ^= _end_angle;
    }

    // calculate the start and end points
    Point start = {(unsigned int)(center.X() + radius * cos(_start_angle * PI / 180)), (unsigned int)(center.Y() + radius * sin(_start_angle * PI / 180))};
    Point end = {(unsigned int)(center.X() + radius * cos(_end_angle * PI / 180)), (unsigned int)(center.Y() + radius * sin(_end_angle * PI / 180))};

    // move Points into local variables
    int xc = center.X();
    int yc = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // loop through the radius
    while (x <= y) {
        // Plot the points within the desired arc range
        if ((x >= _start_angle && x <= _end_angle) || (y >= _start_angle && y <= _end_angle)) 
        {
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
void Graphics::drawFilledArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, unsigned int outer_radius, unsigned int inner_radius, Color color)
{
    
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