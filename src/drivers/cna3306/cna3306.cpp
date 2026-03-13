#include "cna3306.h"
#if defined(LCD_DRIVER_CNA3306)

void cna3306::init()
{    
    // Apply constants
    this->maxWidth = MAX_WIDTH;
    this->maxHeight = MAX_HEIGHT;
}

/**
 * @brief Set the rotation of the display
 * @param rotation Rotation to set
 */
void cna3306::set_rotation(uint32_t rotation)
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
            // this->writeData(0x36, 0x00);
			this->height = height;
			this->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 90:
			// this->writeData(0x36, 0x60);
			this->height = width;
			this->width = height;
            this->maxWidth = MAX_HEIGHT;
            this->maxHeight = MAX_WIDTH;
            break;
        case 180:
			// this->writeData(0x36, 0xc0);
			this->height = height;
			this->width = width;
            this->maxWidth = MAX_WIDTH;
            this->maxHeight = MAX_HEIGHT;
            break;
        case 270:
			// this->writeData(0x36, 0xa0);
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
void cna3306::set_display_state(bool on)
{
    
}
#endif