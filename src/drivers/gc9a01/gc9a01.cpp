#include "gc9a01.h"
#if defined(LCD_DRIVER_GC9A01)

/**
 * @brief Initialize the display
*/
void gc9a01::init()
{
    // Apply constants
    this->maxWidth = MAX_WIDTH;
    this->maxHeight = MAX_HEIGHT;

	// initialize the display
    this->hw->reset(50);
    sleep_ms(100);

	// enable inter register 2
    this->writeData(0xef, (const uint8_t *) NULL, 0);
	// who fucking knows
	this->writeData(0xeb, (const uint8_t *) "\x14", 1);
	// enable inter register 1
    this->writeData(0xfe, (const uint8_t *) NULL, 0);
	// enable inter register 2
	this->writeData(0xef, (const uint8_t *) NULL, 0);
	// who fucking knows
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
	// set the rotation of the display
	this->setRotation(this->rotation);
    // set the display to interface pixel format
    // 0b00000000
    // 7 6 5 4 3 2 1 0
    // | +-+-+------------------ 0b101 = 65k of rgb interface, 0b110 = 262k of rgb interface
    // | | | | | +-+-+----------------- 0b101 = 16 bits per pixel, 0b110 = 18 bits per pixel
    // +-------+----------------------- Set to '0'
#if defined(LCD_COLOR_DEPTH_16)
    const uint8_t pixelFormat = 0x55;   // 65k of rgb interface, 16 bits per pixel
#elif defined(LCD_COLOR_DEPTH_18)
    const uint8_t pixelFormat = 0x66;   // 262k of rgb interface, 18 bits per pixel
#else
#error "Unsupported color depth for ST7789"
#endif
	this->writeData(0x3a, &pixelFormat, 1);
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
	// gamma correction
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
void gc9a01::softReset()
{
	this->writeData(0x01, (const uint8_t *) NULL, 0);
	sleep_ms(120);
}

/**
 * @brief Set the rotation of the display
 * @param rotation Rotation to set
*/
void gc9a01::setRotation(uint32_t rotation)
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
            this->writeData(0x36, 0x48);
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
			this->writeData(0x36, 0x88);
			this->height = height;
			this->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 270:
			this->writeData(0x36, 0xa8);
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
void gc9a01::setDisplayState(bool on)
{
    if(on) this->writeData(0x29);
    else this->writeData(0x28);
    sleep_ms(10);
}
#endif