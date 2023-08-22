#include "GC9A01.hpp"

/**
 * @brief Initialize the display
*/
void GC9A01::init()
{
    // Apply constants
    this->maxWidth = MAX_WIDTH;
    this->maxHeight = MAX_HEIGHT;
	this->config->interface = display_interface_t::DISPLAY_SPI;

	// initialize the display
    this->reset();
    sleep_ms(100);

    this->writeData(0xef, (const uint8_t *) NULL, 0);
	this->writeData(0xeb, (const uint8_t *) "\x14", 1);
    this->writeData(0xfe, (const uint8_t *) NULL, 0);
	this->writeData(0xef, (const uint8_t *) NULL, 0);
	this->writeData(0xeb, (const uint8_t *) "\x14", 1);
	this->writeData(0x84, (const uint8_t *) "\x40", 1);
	this->writeData(0x85, (const uint8_t *) "\xFF", 1);
	this->writeData(0x86, (const uint8_t *) "\xFF", 1);
	this->writeData(0x87, (const uint8_t *) "\xFF", 1);
	this->writeData(0x88, (const uint8_t *) "\x0A", 1);
	this->writeData(0x89, (const uint8_t *) "\x21", 1);
	this->writeData(0x8a, (const uint8_t *) "\x00", 1);
	this->writeData(0x8b, (const uint8_t *) "\x80", 1);
	this->writeData(0x8c, (const uint8_t *) "\x01", 1);
	this->writeData(0x8d, (const uint8_t *) "\x01", 1);
	this->writeData(0x8e, (const uint8_t *) "\xFF", 1);
	this->writeData(0x8f, (const uint8_t *) "\xFF", 1);
	this->writeData(0xb6, (const uint8_t *) "\x00\x00", 2);
	this->setRotation(this->config->rotation);
	this->writeData(0x3a, (const uint8_t *) "\x55", 1);
	this->writeData(0x90, (const uint8_t *) "\x08\x08\x08\x08", 4);
	this->writeData(0xbd, (const uint8_t *) "\x06", 1);
	this->writeData(0xbc, (const uint8_t *) "\x00", 1);
	this->writeData(0xff, (const uint8_t *) "\x60\x01\x04", 3);
	this->writeData(0xc3, (const uint8_t *) "\x13", 1);
	this->writeData(0xc4, (const uint8_t *) "\x13", 1);
	this->writeData(0xc9, (const uint8_t *) "\x22", 1);
	this->writeData(0xbe, (const uint8_t *) "\x11", 1);
	this->writeData(0xe1, (const uint8_t *) "\x10\x0E", 2);
	this->writeData(0xdf, (const uint8_t *) "\x21\x0c\x02", 3);
	this->writeData(0xf0, (const uint8_t *) "\x45\x09\x08\x08\x26\x2A", 6);
 	this->writeData(0xf1, (const uint8_t *) "\x43\x70\x72\x36\x37\x6F", 6);
 	this->writeData(0xf2, (const uint8_t *) "\x45\x09\x08\x08\x26\x2A", 6);
 	this->writeData(0xf3, (const uint8_t *) "\x43\x70\x72\x36\x37\x6F", 6);
	this->writeData(0xed, (const uint8_t *) "\x1B\x0B", 2);
	this->writeData(0xae, (const uint8_t *) "\x77", 1);
	this->writeData(0xcd, (const uint8_t *) "\x63", 1);
	this->writeData(0x70, (const uint8_t *) "\x07\x07\x04\x0E\x0F\x09\x07\x08\x03", 9);
	this->writeData(0xe8, (const uint8_t *) "\x34", 1);
	this->writeData(0x62, (const uint8_t *) "\x18\x0D\x71\xED\x70\x70\x18\x0F\x71\xEF\x70\x70", 12);
	this->writeData(0x63, (const uint8_t *) "\x18\x11\x71\xF1\x70\x70\x18\x13\x71\xF3\x70\x70", 12);
	this->writeData(0x64, (const uint8_t *) "\x28\x29\xF1\x01\xF1\x00\x07", 7);
	this->writeData(0x66, (const uint8_t *) "\x3C\x00\xCD\x67\x45\x45\x10\x00\x00\x00", 10);
	this->writeData(0x67, (const uint8_t *) "\x00\x3C\x00\x00\x00\x01\x54\x10\x32\x98", 10);
	this->writeData(0x74, (const uint8_t *) "\x10\x85\x80\x00\x00\x4E\x00", 7);
    this->writeData(0x98, (const uint8_t *) "\x3e\x07", 2);
	this->writeData(0x35, (const uint8_t *) NULL, 0);
	this->writeData(0x21, (const uint8_t *) NULL, 0);
	this->writeData(0x11, (const uint8_t *) NULL, 0);
    sleep_ms(120);

	this->writeData(0x29, (const uint8_t *) NULL, 0);
	sleep_ms(20);

    // clear the display
    this->clear();
}

/**
 * @private
 * @brief Reset the display, but not the chip
*/
void GC9A01::softReset()
{
	this->writeData(0x01, (const uint8_t *) NULL, 0);
	sleep_ms(120);
}

/**
 * @brief Reset the display
*/
void GC9A01::reset()
{
	// Hardware reset
	gpio_put(this->config->spi.rst, 1);
	sleep_ms(50);
	gpio_put(this->config->spi.rst, 0);
	sleep_ms(50);
	gpio_put(this->config->spi.rst, 1);
}

/**
 * @brief Set the rotation of the display
 * @param rotation Rotation to set
*/
void GC9A01::setRotation(int rotation)
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
            this->writeData(0x36, 0x48);
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
			this->writeData(0x36, 0x88);
			this->config->height = height;
			this->config->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 3:
			this->writeData(0x36, 0xa8);
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
void GC9A01::setDisplayState(bool on)
{
    if(on) this->writeData(0x29);
    else this->writeData(0x28);
    sleep_ms(10);
}