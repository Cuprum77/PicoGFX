#include "graphics.h"

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void graphics::drawBitmap(const uint8_t *bitmap, uint32_t width, uint32_t height)
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
void graphics::drawBitmap(const uint16_t *bitmap, uint32_t width, uint32_t height)
{
    this->drawBitmap(bitmap, width, height, point(0, 0));
}

/**
 * @brief Draw a 18/24 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void graphics::drawBitmap(const uint32_t *bitmap, uint32_t width, uint32_t height)
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
void graphics::drawBitmap(const uint8_t *bitmap, uint32_t width, uint32_t height, bool center)
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
void graphics::drawBitmap(const uint16_t *bitmap, uint32_t width, uint32_t height, bool center)
{
    int startX = 0;
    int startY = 0;

    if (width != this->display_ptr->getWidth())
        startX = (int)(this->display_ptr->getWidth() - width) >> 1;

    if (height != this->display_ptr->getHeight())
        startY = (int)(this->display_ptr->getHeight() - height) >> 1;

    point start = point(startX, startY);
    this->drawBitmap(bitmap, width, height, start);
}
/**
 * @brief Draw a 18/24 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param center If the image should be centered on the screen or not (Default: true)
*/
void graphics::drawBitmap(const uint32_t *bitmap, uint32_t width, uint32_t height, bool center)
{
    int startX = 0;
    int startY = 0;

    if (width != this->display_ptr->getWidth())
        startX = (int)(this->display_ptr->getWidth() - width) >> 1;

    if (height != this->display_ptr->getHeight())
        startY = (int)(this->display_ptr->getHeight() - height) >> 1;

    point start = point(startX, startY);
    this->drawBitmap(bitmap, width, height, start);
}

/**
 * @brief Draw a 8 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param start Where to start the drawing, defaults to (0,0)
*/
void graphics::drawBitmap(const uint8_t *bitmap, uint32_t width, uint32_t height, point start)
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
void graphics::drawBitmap(const uint16_t *bitmap, uint32_t width, uint32_t height, point start)
{
    int startX = start.x;
    int startY = start.y;

    if (startX >= (int)this->display_ptr->getWidth() || startY >= (int)this->display_ptr->getHeight() 
        || startX + (int)width <= 0 || startY + (int)height <= 0)
        return;

    int offsetX = imax(-startX, 0);
    int offsetY = imax(-startY, 0);

    int endX = imin(startX + (int)width, (int)this->display_ptr->getWidth());
    int endY = imin(startY + (int)height, (int)this->display_ptr->getHeight());

    for (int y = startY + offsetY, by = offsetY; y < endY; ++y, ++by)
    {
        for (int x = startX + offsetX, bx = offsetX; x < endX; ++x, ++bx)
        {
            uint16_t colorWord = bitmap[by * width + bx];
            
#if defined(LCD_INVERT_COLORS)
            colorWord = COLOR_INV(colorWord);
#endif
            this->frameBuffer[y * this->display_ptr->getWidth() + x] = colorWord;
        }
    }
}

/**
 * @brief Draw a 18/24 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
 * @param start Where to start the drawing, defaults to (0,0)
*/
void graphics::drawBitmap(const uint32_t *bitmap, uint32_t width, uint32_t height, point start)
{
    int startX = start.x;
    int startY = start.y;

    if (startX >= (int)this->display_ptr->getWidth() || startY >= (int)this->display_ptr->getHeight() 
        || startX + (int)width <= 0 || startY + (int)height <= 0)
        return;

    int offsetX = imax(-startX, 0);
    int offsetY = imax(-startY, 0);

    int endX = imin(startX + (int)width, (int)this->display_ptr->getWidth());
    int endY = imin(startY + (int)height, (int)this->display_ptr->getHeight());

    for (int y = startY + offsetY, by = offsetY; y < endY; ++y, ++by)
    {
        for (int x = startX + offsetX, bx = offsetX; x < endX; ++x, ++bx)
        {
            uint32_t colorWord = 0;
            for (int i = 0; i < 3; ++i)
            {
                uint8_t colorByte = ((const uint8_t*)bitmap)[(by * width + bx) * 3 + i];
                colorWord |= colorByte << ((2 - i) * 8);
            }
            colorWord = COLOR_INV(colorWord);
            this->frameBuffer[y * this->display_ptr->getWidth() + x] = colorWord;
        }
    }
}