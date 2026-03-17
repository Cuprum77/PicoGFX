#include "st7262.h"
#if defined(LCD_DRIVER_ST7262)

/**
 * @brief Initialize the display
*/
void st7262::init()
{
    // Apply constants
    this->maxWidth = LCD_WIDTH;
    this->maxHeight = LCD_HEIGHT;

    // clear the display
    this->clear();
}

/**
 * @brief Set the rotation of the display
 * @param rotation Rotation to set
*/
void st7262::set_rotation(uint32_t rotation)
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
			this->height = height;
			this->width = width;
            this->maxWidth = LCD_WIDTH;
            this->maxHeight = LCD_HEIGHT;
            break;
        case 90:
			this->height = width;
			this->width = height;
            this->maxWidth = LCD_HEIGHT;
            this->maxHeight = LCD_WIDTH;
            break;
        case 180:
			this->height = height;
			this->width = width;
            this->maxWidth = LCD_WIDTH;
            this->maxHeight = LCD_HEIGHT;
            break;
        case 270:
			this->height = width;
			this->width = height;
            this->maxWidth = LCD_HEIGHT;
            this->maxHeight = LCD_WIDTH;
            break;
        default:
            return;
    }
}
#endif