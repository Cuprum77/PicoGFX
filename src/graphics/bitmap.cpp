#include "graphics.hpp"

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void graphics::drawBitmap(const uint8_t* bitmap, uint32_t width, uint32_t height)
{
    this->drawBitmap((const uint16_t*)bitmap, width, height, point(0,0));
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void graphics::drawBitmap(const uint16_t* bitmap, uint32_t width, uint32_t height)
{
    this->drawBitmap(bitmap, width, height, point(0, 0));
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param center If the image should be centered on the screen or not (Default: true)
*/
void graphics::drawBitmap(const uint8_t* bitmap, uint32_t width, uint32_t height, bool center)
{
    this->drawBitmap((const uint16_t*)bitmap, width, height, true);
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param center If the image should be centered on the screen or not (Default: true)
*/
void graphics::drawBitmap(const uint16_t* bitmap, uint32_t width, uint32_t height, bool center)
{
    int startX = 0;
    int startY = 0;

    if (width != this->config->width)
        startX = (int)(this->config->width - width) >> 1;

    if (height != this->config->height)
        startY = (int)(this->config->height - height) >> 1;

    point start = point(startX, startY);
    this->drawBitmap(bitmap, width, height, start);
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param start Where to start the drawing, defaults to (0,0)
*/
void graphics::drawBitmap(const uint8_t* bitmap, uint32_t width, uint32_t height, point start)
{
    this->drawBitmap((const uint16_t*)bitmap, width, height, start);
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param start Where to start the drawing, defaults to (0,0)
*/
void graphics::drawBitmap(const uint16_t* bitmap, uint32_t width, uint32_t height, point start)
{
    int startX = start.x;
    int startY = start.y;

    if (startX >= (int)this->config->width || startY >= (int)this->config->height 
        || startX + (int)width <= 0 || startY + (int)height <= 0)
        return;

    int offsetX = imax(-startX, 0);
    int offsetY = imax(-startY, 0);

    int endX = imin(startX + (int)width, (int)this->config->width);
    int endY = imin(startY + (int)height, (int)this->config->height);

    for (int y = startY + offsetY, by = offsetY; y < endY; ++y, ++by)
    {
        for (int x = startX + offsetX, bx = offsetX; x < endX; ++x, ++bx)
        {
            uint16_t color16 = bitmap[by * width + bx];
            if (this->config->inverseColors)
            {
                color16 = ((color16 & 0xaaaa) >> 1) | ((color16 & 0x5555) << 1);
                color16 = ((color16 & 0xcccc) >> 2) | ((color16 & 0x3333) << 2);
                color16 = ((color16 & 0xf0f0) >> 4) | ((color16 & 0x0f0f) << 4);
                color16 = (color16 >> 8) | (color16 << 8);
            }

            this->frameBuffer[y * this->config->width + x] = color16;
        }
    }
}