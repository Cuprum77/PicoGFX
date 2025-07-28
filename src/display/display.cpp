#include "display.hpp"

/**
 * @brief display initialization
*/
display::display(hardware_driver* hw, display_config_t* config, uint16_t* frameBuffer, uint8_t CASET, uint8_t RASET, uint8_t RAMWR)
{
    this->hw = hw;
    this->config = config;
    this->totalPixels = config->width * config->height;
    this->frameBuffer = frameBuffer;
    this->CASET = CASET;
    this->RASET = RASET;
    this->RAMWR = RAMWR;

    // init the rest of the pins if we are using a compatible screen
    if (config->interface == display_interface_t::DISPLAY_SPI)
    {
        gpio_init(this->config->spi.rst);
        // set the rest of the pins to GPIO output
        gpio_set_dir(this->config->spi.rst, GPIO_OUT);
        // set the pins to high
        gpio_put(this->config->spi.rst, 1);
    }

    // set up the backlight pin depending on the dimming setting
    this->backlight = this->config->backlightPin != -1;
    if (this->backlight)
    {
        if (config->dimming)
        {
            // enable dimming
            this->dimmingEnabled = true;
            gpio_set_function(this->config->backlightPin, GPIO_FUNC_PWM);
            // get the PWM slice number
            uint32_t sliceNum = pwm_gpio_to_slice_num(this->config->backlightPin);
            this->sliceNum = sliceNum;
            // get the PWM channel
            uint32_t chan = pwm_gpio_to_channel(this->config->backlightPin);
            this->pwmChannel = chan;
            // turn on the PWM slice
            pwm_set_enabled(sliceNum, true);
            // set the PWM wrap value
            pwm_set_wrap(sliceNum, 255);
            // set the PWM value
            pwm_set_chan_level(sliceNum, chan, 127);
        }
        else
        {
            this->dimmingEnabled = false;
            gpio_init(this->config->backlightPin);
            gpio_set_dir(this->config->backlightPin, GPIO_OUT);
            gpio_put(this->config->backlightPin, 1);
        }
    }
}

/**
 * @brief Clear the display by drawing a black rectangle
*/
void display::clear()
{
    // set the cursor position to the top left
    this->setCursor({ 0, 0 });
    // fill the frame buffer
    for (int32_t i = 0; i < this->totalPixels; i++)
        this->frameBuffer[i] = 0x0000;
    this->setCursor({ 0, 0 });
    this->update();
}

/**
 * @brief Print the frame buffer to the display
 */
void display::update()
{
    this->setCursor({ 0, 0 });
    this->writePixels(this->frameBuffer, this->totalPixels);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start index
 * @param end End index
 * @note No checks are done to speed up the process, make sure the start and end are valid!
*/
void display::update(int32_t start, int32_t end)
{
    this->writePixels(&this->frameBuffer[start], end - start);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start index
 * @param end End index
 * @param moveCursor Move the cursor to the start position
*/
void display::update(int32_t start, int32_t end, bool moveCursor)
{
    // Check if the start and end are valid
    if (start >= end || end >= this->totalPixels)
        return;

    // Move the cursor if needed
    if (moveCursor)
        this->setCursor({ start % this->config->width, start / this->config->width });

    // Write the pixels
    this->writePixels(&this->frameBuffer[start], end - start);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start point
 * @param end End point
*/
void display::update(point start, point end)
{
    // Check if the start and end are valid
    if (end.x < start.x || end.y < start.y || start.x < 0 || start.y < 0)
        return;
    if (end.x >= this->config->width || end.y >= this->config->height)
        return;

    // Move cursor
    this->setCursor(start);

    // Calculate the start and end index
    int32_t startIndex = start.x + (start.y * (this->config->width));
    int32_t endIndex = end.x + (end.y * (this->config->width));
    
    // Write the pixels
    this->update(startIndex, endIndex);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param rect Rectangle to update
*/
void display::update(rect r)
{
    // Check if the start and end are valid
    if (r.left() < 0 || r.top() < 0)
        return;
        
    // clamp the box to the display
    point min = point(0, 0);
    point max = point(this->config->width - 1, this->config->height - 1);
    r = r.clamp(min, max);
        
    for (int32_t y = r.top(); y <= r.bottom(); y++)
    {
        this->update({r.left(), y}, {r.right(), y});
    }
}

/**
 * @brief Run after each frame to calculate the framerate
*/
void display::frameCounter()
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
bool display::frameLimiter(uint32_t frameRate)
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
void display::setPixel(point point, color color)
{
    // set the framebuffer pixel
    this->frameBuffer[point.x + point.y * this->config->width] = color.to16bit(this->config->inverseColors);
}

/**
 * @brief Put a pixel in the framebuffer
 * @param point Buffer index
 * @param color Color to draw in as a 16 bit value
*/
void display::setPixel(uint32_t index, uint16_t color)
{
    // set the framebuffer pixel
    this->frameBuffer[index] = color;
}

/**
 * @brief Get a pixel from the framebuffer
 * @param point point to get the pixel from
 * @return Color The color of the pixel
*/
color display::getPixel(point point)
{
    return color(this->frameBuffer[point.x + point.y * this->config->width]);
}

/**
 * @brief Get a pixel from the framebuffer
 * @param point Buffer index
 * @return Color The color of the pixel
*/
uint16_t display::getPixel(uint32_t index)
{
    return this->frameBuffer[index];
}

/**
 * @brief Set the cursor position
 * @param point point to set the cursor to
*/
void display::setCursor(point point)
{
    // set the pixel x address
    this->columnAddressSet(
        point.x + this->config->columnOffset1,
        (this->config->width - 1) + this->config->columnOffset2
    );
    // set the pixel y address
    this->rowAddressSet(
        point.y + this->config->rowOffset1,
        (this->config->height - 1) + this->config->rowOffset2
    );
    // set the internal cursor position
    this->cursor = point;
}

/**
 * @brief Get the cursor position
 * @return point The cursor position
*/
point display::getCursor()
{
    return this->cursor;
}

/**
 * @brief Get the center of the display
 * @return point The center of the display
*/
point display::getCenter()
{
    point point = {
        this->config->width / 2,
        this->config->height / 2
    };
    return point;
}

/**
 * @brief Set the backlight brightness
 * @param brightness Brightness (0-255) if dimming is enabled, brightness (0-1) if dimming is disabled
*/
void display::setBrightness(uint8_t brightness)
{
    if (!this->backlight)
        return;

    // check if dimming is enabled
    if (this->dimmingEnabled)
    {
        // set the brightness
        pwm_set_chan_level(this->sliceNum, this->pwmChannel, brightness);
    }
    else
    {
        // make sure the brightness is between 0 and 1
        brightness = brightness & 0x01;
        // toggle the backlight pin based on the brightness
        gpio_put(this->config->backlightPin, brightness);
    }
}


/**
 * @private
 * @brief Write data to the display
 * @param command Command to send
 * @param data Data to send
 * @param length Length of the data
*/
void display::writeData(uint8_t command, const uint8_t* data, size_t length)
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
inline void display::columnAddressSet(uint32_t x0, uint32_t x1)
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
inline void display::rowAddressSet(uint32_t y0, uint32_t y1)
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

/**
 * @private
 * @brief Write pixels to the display
 * @param data data to write
 * @param length Length of the data
 * @note length should be number of 16 bit pixels, not bytes!
*/
void display::writePixels(const uint16_t* data, size_t length)
{
    // check if the data mode is set
    if (!this->dataMode)
    {
        // set the data mode
        this->hw->setDataMode(this->RAMWR);
        this->dataMode = true;
    }
    // write the pixels
    this->hw->writePixels(data, length);
}
