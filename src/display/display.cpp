#include "display.h"

/**
 * @brief display initialization
*/
display_obj::display_obj(hardware_driver *hw, void *frameBuffer, uint8_t CASET, uint8_t RASET, uint8_t RAMWR)
{
    this->hw = hw;
#if defined(LCD_COLOR_DEPTH_1)
    this->frameBuffer = (bool *)frameBuffer;
#elif defined(LCD_COLOR_DEPTH_8)
    this->frameBuffer = (uint8_t *)frameBuffer;
#elif defined(LCD_COLOR_DEPTH_16)
    this->frameBuffer = (uint16_t *)frameBuffer;
#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
    this->frameBuffer = (uint32_t *)frameBuffer;
#endif
    this->CASET = CASET;
    this->RASET = RASET;
    this->RAMWR = RAMWR;
    this->width = LCD_WIDTH;
    this->height = LCD_HEIGHT;
}

/**
 * @brief Clear the display by drawing a black rectangle
*/
void display_obj::clear()
{
    // set the cursor position to the top left
    this->setCursor({ 0, 0 });
    // fill the frame buffer
    uint32_t totalPixels = this->width * this->height;
    for (int32_t i = 0; i < totalPixels; i++)
        this->frameBuffer[i] = COLOR_BLACK;
    this->setCursor({ 0, 0 });
    this->update();
}

/**
 * @brief Print the frame buffer to the display
 */
void display_obj::update()
{
    this->setCursor({ 0, 0 });
    uint32_t totalPixels = this->width * this->height;
    this->writePixels(this->frameBuffer, totalPixels);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start index
 * @param end End index
 * @note No checks are done to speed up the process, make sure the start and end are valid!
*/
void display_obj::update(int32_t start, int32_t end)
{
    this->writePixels(&this->frameBuffer[start], end - start);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start index
 * @param end End index
 * @param moveCursor Move the cursor to the start position
*/
void display_obj::update(int32_t start, int32_t end, bool moveCursor)
{
    uint32_t totalPixels = this->width * this->height;

    // Check if the start and end are valid
    if (start >= end || end >= totalPixels)
        return;

    // Move the cursor if needed
    if (moveCursor)
        this->setCursor({ start % this->width, start / this->width });

    // Write the pixels
    this->writePixels(&this->frameBuffer[start], end - start);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start point
 * @param end End point
*/
void display_obj::update(point start, point end)
{
    // Check if the start and end are valid
    if (end.x < start.x || end.y < start.y || start.x < 0 || start.y < 0)
        return;
    if (end.x >= this->width || end.y >= this->height)
        return;

    // Move cursor
    this->setCursor(start);

    // Calculate the start and end index
    int32_t startIndex = start.x + (start.y * (this->width));
    int32_t endIndex = end.x + (end.y * (this->width));
    
    // Write the pixels
    this->update(startIndex, endIndex);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param rect Rectangle to update
*/
void display_obj::update(rect r)
{
    // Check if the start and end are valid
    if (r.left() < 0 || r.top() < 0)
        return;
        
    // clamp the box to the display
    point min = point(0, 0);
    point max = point(this->width - 1, this->height - 1);
    r = r.clamp(min, max);
        
    for (int32_t y = r.top(); y <= r.bottom(); y++)
    {
        this->update({r.left(), y}, {r.right(), y});
    }
}

/**
 * @brief Run after each frame to calculate the framerate
*/
void display_obj::frameCounter()
{
    this->framecounter++;
    if ((time_us_64() - this->timer) >= 1000000)
    {
        this->timer = time_us_64();
        this->frames = this->framecounter;
        this->framecounter = 0;
    }
}

/**
 * @brief Limit the frame rate
 * @param frameRate Frame rate to limit to
 * @return bool True if the next frame can be drawn
*/
bool display_obj::frameLimiter(uint32_t frameRate)
{
    if (frameRate == 0)
        return true;

    if ((time_us_64() - this->lastFrame) >= (1'000'000 / frameRate))
    {
        this->lastFrame = time_us_64();
        return true;
    }

    return false;
}

/**
 * @brief Put a pixel in the framebuffer
 * @param point Points to draw the pixel at
 * @param color Color to draw in
*/
void display_obj::setPixel(point point, color color)
{
    // set the framebuffer pixel
    this->frameBuffer[point.x + point.y * this->width] = color.toWord();
}

/**
 * @brief Put a pixel in the framebuffer
 * @param point Buffer index
 * @param color Color to draw in as a 16 bit value
*/
void display_obj::setPixel(uint32_t index, uint16_t color)
{
    // set the framebuffer pixel
    this->frameBuffer[index] = color;
}

/**
 * @brief Get a pixel from the framebuffer
 * @param point point to get the pixel from
 * @return Color The color of the pixel
*/
color display_obj::getPixel(point point)
{
    return color(this->frameBuffer[point.x + point.y * this->width]);
}

#if defined(LCD_COLOR_DEPTH_1)
/**
 * @brief Get a pixel from the framebuffer
 * @param point Buffer index
 * @return Color The color of the pixel as a bool
*/
bool display_obj::getPixel(uint32_t index)
{
    return this->frameBuffer[index];
}

/**
 * @private
 * @brief Write pixels to the display
 * @param data data to write
 * @param length Length of the data
 * @note length should be number of 16 bit pixels, not bytes!
*/
void display_obj::writePixels(const bool *data, size_t length)
{
    // check if the data mode is set
    if (!this->dataMode)
    {
        // set the data mode
        this->hw->setDataMode(this->RAMWR);
        this->dataMode = true;
    }
    // write the pixels
    this->hw->writePixels((const bool *)data, length);
}

#elif defined(LCD_COLOR_DEPTH_8)
/**
 * @brief Get a pixel from the framebuffer
 * @param point Buffer index
 * @return Color The color of the pixel as an 8 bit value
*/
uint8_t display_obj::getPixel(uint32_t index)
{
    return this->frameBuffer[index];
}

/**
 * @private
 * @brief Write pixels to the display
 * @param data data to write
 * @param length Length of the data
 * @note length should be number of 16 bit pixels, not bytes!
*/
void display_obj::writePixels(const uint8_t *data, size_t length)
{
    // check if the data mode is set
    if (!this->dataMode)
    {
        // set the data mode
        this->hw->setDataMode(this->RAMWR);
        this->dataMode = true;
    }
    // write the pixels
    this->hw->writePixels((const uint8_t *)data, length);
}

#elif defined(LCD_COLOR_DEPTH_16)
/**
 * @brief Get a pixel from the framebuffer
 * @param point Buffer index
 * @return Color The color of the pixel as a 16 bit value
*/
uint16_t display_obj::getPixel(uint32_t index)
{
    return this->frameBuffer[index];
}

/**
 * @private
 * @brief Write pixels to the display
 * @param data data to write
 * @param length Length of the data
 * @note length should be number of 16 bit pixels, not bytes!
*/
void display_obj::writePixels(const uint16_t *data, size_t length)
{
    // check if the data mode is set
    if (!this->dataMode)
    {
        // set the data mode
        this->hw->setDataMode(this->RAMWR);
        this->dataMode = true;
    }
    // write the pixels
    this->hw->writePixels((const uint16_t *)data, length);
}

#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
/**
 * @brief Get a pixel from the framebuffer
 * @param point Buffer index
 * @return Color The color of the pixel as a 18 or 24 bit value
*/
uint32_t display_obj::getPixel(uint32_t index)
{
    return this->frameBuffer[index];
}

/**
 * @private
 * @brief Write pixels to the display
 * @param data data to write
 * @param length Length of the data
 * @note length should be number of 16 bit pixels, not bytes!
*/
void display_obj::writePixels(const uint32_t *data, size_t length)
{
    // check if the data mode is set
    if (!this->dataMode)
    {
        // set the data mode
        this->hw->setDataMode(this->RAMWR);
        this->dataMode = true;
    }
    // write the pixels
    this->hw->writePixels((const uint32_t *)data, length);
}

#endif

/**
 * @brief Set the cursor position
 * @param point point to set the cursor to
*/
void display_obj::setCursor(point point)
{
    // set the pixel x address
    this->columnAddressSet(
        point.x + this->columnOffset1,
        (this->width - 1) + this->columnOffset2
    );
    // set the pixel y address
    this->rowAddressSet(
        point.y + this->rowOffset1,
        (this->height - 1) + this->rowOffset2
    );
    // set the internal cursor position
    this->cursor = point;
}

/**
 * @brief Get the cursor position
 * @return point The cursor position
*/
point display_obj::getCursor()
{
    return this->cursor;
}

/**
 * @brief Get the center of the display
 * @return point The center of the display
*/
point display_obj::getCenter()
{
    point point = {
        this->width / 2,
        this->height / 2
    };
    return point;
}

/**
 * @private
 * @brief Write data to the display
 * @param command Command to send
 * @param data Data to send
 * @param length Length of the data
*/
void display_obj::writeData(uint8_t command, const uint8_t *data, size_t length)
{
    // set the data mode
    this->dataMode = false;
    // write the command
    this->hw->writeData(command, data, length);
}

/**
 * @private
 * @brief Set the column address
 * @param x0 Start column
 * @param x1 End column
*/
inline void display_obj::columnAddressSet(uint32_t x0, uint32_t x1)
{
    // deny out of bounds
    if (x0 >= x1 || x1 >= this->maxWidth)
        return;

    // pack the data
    uint8_t data[4] = {
        (uint8_t)(x0 >> 8),
        (uint8_t)(x0 & 0xff),
        (uint8_t)(x1 >> 8),
        (uint8_t)(x1 & 0xff)
    };

    // write the data
    this->writeData(this->CASET, data, sizeof(data));
}

/**
 * @private
 * @brief Set the row address
 * @param y0 Start row
 * @param y1 End row
*/
inline void display_obj::rowAddressSet(uint32_t y0, uint32_t y1)
{
    // deny out of bounds
    if (y0 >= y1 || y1 >= this->maxHeight)
        return;

    // pack the data
    uint8_t data[4] = {
        (uint8_t)(y0 >> 8),
        (uint8_t)(y0 & 0xff),
        (uint8_t)(y1 >> 8),
        (uint8_t)(y1 & 0xff)
    };

    // write the data
    this->writeData(this->RASET, data, sizeof(data));
}