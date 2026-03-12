#include "st7789.hpp"

void st7789::init()
{    
    // Apply constants
    this->maxWidth = MAX_WIDTH;
    this->maxHeight = MAX_HEIGHT;

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
    this->setRotation(this->rotation);

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
void st7789::setRotation(uint32_t rotation)
{
	// save the rotation
    this->rotation = rotation;
    uint32_t width = this->width;
	uint32_t height = this->height;
    uint32_t maxWidth = this->maxWidth;
    uint32_t maxHeight = this->maxHeight;

	switch(rotation)
    {
		case 0:
            this->writeData(0x36, 0x00);
			this->height = height;
			this->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 90:
			this->writeData(0x36, 0x60);
			this->height = width;
			this->width = height;
            this->maxWidth = MAX_HEIGHT;
            this->maxHeight = MAX_WIDTH;
            break;
        case 180:
			this->writeData(0x36, 0xc0);
			this->height = height;
			this->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 270:
			this->writeData(0x36, 0xa0);
			this->height = width;
			this->width = height;
            this->maxWidth = MAX_HEIGHT;
            this->maxHeight = MAX_WIDTH;
            break;
        default:
            break;
    }
}

/**
 * @brief Turn the display on
*/
void st7789::setDisplayState(bool on)
{
    if(on) this->writeData(0x29);
    else this->writeData(0x28);
    sleep_ms(10);
}