#include "ST7789.hpp"

void ST7789::init()
{    
    // Apply constants
    this->maxWidth = MAX_WIDTH;
    this->maxHeight = MAX_HEIGHT;
    this->config->interface = display_interface_t::DISPLAY_SPI;

    // reset the display
    this->writeData(0x01, NULL, 0);
    sleep_ms(100);

    // set the display to sleep out
    this->writeData(0x11, NULL, 0);
    sleep_ms(50);

    // set the display to interface pixel format
    // 0x5 << 4 = 65k of rgb interface
    // 0x5 = 16 bits per pixel
    unsigned char pixelFormat = 0x5 << 4 | 0x5;
    this->writeData(0x3a, &pixelFormat, 1);
    sleep_ms(10);

    // madctl = memory access control
    this->setRotation(this->config->rotation);

    // set the display to memory access control
    this->setCursor({0, 0});

    // display inversion on
    this->writeData(0x21, NULL, 0);
    sleep_ms(10);

    // normal display mode on
    this->writeData(0x13, NULL, 0);
    sleep_ms(10);

    // clear the display
    this->clear();

    // turn on the display
    this->writeData(0x29, (const uint8_t*)NULL, 0);
}

/**
 * @brief Set the rotation of the display
 * @param rotation Rotation to set
 */
void ST7789::setRotation(int rotation)
{
    rotation %= 4; // Normalize rotation to 0-3
	// save the rotation
    this->config->rotation = (int)rotation;
    unsigned int width = this->config->width;
	unsigned int height = this->config->height;
    unsigned int maxWidth = this->maxWidth;
    unsigned int maxHeight = this->maxHeight;

	switch(rotation)
    {
		case 0:
            this->writeData(0x36, 0x00);
			this->config->height = height;
			this->config->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 1:
			this->writeData(0x36, 0x60);
			this->config->height = width;
			this->config->width = height;
            this->maxWidth = MAX_HEIGHT;
            this->maxHeight = MAX_WIDTH;
            break;
        case 2:
			this->writeData(0x36, 0xc0);
			this->config->height = height;
			this->config->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 3:
			this->writeData(0x36, 0xa0);
			this->config->height = width;
			this->config->width = height;
            this->maxWidth = MAX_HEIGHT;
            this->maxHeight = MAX_WIDTH;
            break;
    }
}

/**
 * @brief Turn the display on
*/
void ST7789::setDisplayState(bool on)
{
    if(on) this->writeData(0x29);
    else this->writeData(0x28);
    sleep_ms(10);
}