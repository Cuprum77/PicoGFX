#include <Display.hpp>

/**
 * @brief Display initialization
*/
Display::Display(Driver* spi, display_config_t* config, unsigned short* frameBuffer, unsigned char CASET, unsigned char RASET, unsigned char RAMWR)
{
    this->spi = spi;
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
            uint sliceNum = pwm_gpio_to_slice_num(this->config->backlightPin);
            this->sliceNum = sliceNum;
            // get the PWM channel
            uint chan = pwm_gpio_to_channel(this->config->backlightPin);
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
void Display::clear()
{
    // set the cursor position to the top left
    this->setCursor({ 0, 0 });
    // fill the frame buffer
    for (int i = 0; i < this->totalPixels; i++)
        this->frameBuffer[i] = 0x0000;
    this->setCursor({ 0, 0 });
    this->update();
}

/**
 * @brief Print the frame buffer to the display
*/
void Display::update(bool framecounter)
{
    this->setCursor({ 0, 0 });
    this->writePixels(this->frameBuffer, this->totalPixels);
    if (!framecounter) return;

    this->framecounter++;
    if ((time_us_64() - this->timer) >= 1000000)
    {
        this->timer = time_us_64();
        this->frames = this->framecounter;
        this->framecounter = 0;
    }
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start index
 * @param end End index
 * @note No checks are done to speed up the process, make sure the start and end are valid!
*/
void Display::update(int start, int end)
{
    this->writePixels(&this->frameBuffer[start], end - start);
}

/**
 * @brief Partially update the display with a section of the frame buffer
 * @param start Start index
 * @param end End index
 * @param moveCursor Move the cursor to the start position
*/
void Display::update(int start, int end, bool moveCursor)
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
 * @brief Put a pixel in the framebuffer
 * @param Point Points to draw the pixel at
 * @param color Color to draw in
*/
void Display::setPixel(Point point, Color color)
{
    // set the framebuffer pixel
    this->frameBuffer[point.x + point.y * this->config->width] = color.to16bit();
}

/**
 * @brief Put a pixel in the framebuffer
 * @param Point Buffer index
 * @param color Color to draw in as a 16 bit value
*/
void Display::setPixel(uint index, ushort color)
{
    // set the framebuffer pixel
    this->frameBuffer[index] = color;
}

/**
 * @brief Get a pixel from the framebuffer
 * @param Point Point to get the pixel from
 * @return Color The color of the pixel
*/
Color Display::getPixel(Point point)
{
    return Color(this->frameBuffer[point.x + point.y * this->config->width]);
}

/**
 * @brief Get a pixel from the framebuffer
 * @param Point Buffer index
 * @return Color The color of the pixel
*/
ushort Display::getPixel(uint index)
{
    return this->frameBuffer[index];
}

/**
 * @brief Set the cursor position
 * @param Point Point to set the cursor to
*/
void Display::setCursor(Point point)
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
 * @return Point The cursor position
*/
Point Display::getCursor()
{
    return this->cursor;
}

/**
 * @brief Get the center of the display
 * @return Point The center of the display
*/
Point Display::getCenter()
{
    Point Point = {
        this->config->width / 2,
        this->config->height / 2
    };
    return Point;
}

/**
 * @brief Set the backlight brightness
 * @param brightness Brightness (0-255) if dimming is enabled, brightness (0-1) if dimming is disabled
*/
void Display::setBrightness(unsigned char brightness)
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
void Display::writeData(unsigned char command, const unsigned char* data, size_t length)
{
    // set the data mode
    this->dataMode = false;
    // write the command
    this->spi->writeData(command, data, length);
}

/**
 * @private
 * @brief Set the column address
 * @param x0 Start column
 * @param x1 End column
*/
inline void Display::columnAddressSet(uint x0, uint x1)
{
    // deny out of bounds
    if (x0 >= x1 || x1 >= this->maxWidth)
        return;

    // pack the data
    unsigned char data[4] = {
        (unsigned char)(x0 >> 8),
        (unsigned char)(x0 & 0xff),
        (unsigned char)(x1 >> 8),
        (unsigned char)(x1 & 0xff)
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
inline void Display::rowAddressSet(uint y0, uint y1)
{
    // deny out of bounds
    if (y0 >= y1 || y1 >= this->maxHeight)
        return;

    // pack the data
    unsigned char data[4] = {
        (unsigned char)(y0 >> 8),
        (unsigned char)(y0 & 0xff),
        (unsigned char)(y1 >> 8),
        (unsigned char)(y1 & 0xff)
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
void Display::writePixels(const unsigned short* data, size_t length)
{
    // check if the data mode is set
    if (!this->dataMode)
    {
        // set the data mode
        this->spi->setDataMode(this->RAMWR);
        this->dataMode = true;
    }
    // write the pixels
    this->spi->writePixels(data, length);
}