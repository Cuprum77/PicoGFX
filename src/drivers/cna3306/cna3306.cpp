#include "cna3306.h"
#if defined(LCD_DRIVER_CNA3306)

void cna3306::init() 
{ 
    this->maxWidth  = LCD_WIDTH; 
    this->maxHeight = LCD_HEIGHT; 

    // Software reset 
    this->writeData(0x01, nullptr, 0); 
    sleep_ms(130); 

    // Unlock extended commands 
    this->writeData(0xfe, (const uint8_t*)"\x00", 1); 

    // Enable commands 
    this->writeData(0xc0, (const uint8_t*)"\x5a\x5a", 2); 
    this->writeData(0xc1, (const uint8_t*)"\x5a\x5a", 2); 

    // Sleep out 
    this->writeData(0x11, nullptr, 0); 
    sleep_ms(130); 

#if defined(LCD_COLOR_DEPTH_8) 
    this->writeData(0x3a, (const uint8_t*)"\x22", 1); 
#elif defined(LCD_COLOR_DEPTH_16) 
    this->writeData(0x3a, (const uint8_t*)"\x55", 1); 
#elif defined(LCD_COLOR_DEPTH_18) 
    this->writeData(0x3a, (const uint8_t*)"\x66", 1); 
#elif defined(LCD_COLOR_DEPTH_24) 
    this->writeData(0x3a, (const uint8_t*)"\x77", 1); 
#endif 

    this->set_rotation(this->rotation); 

    // Enable QSPI interface 
    this->writeData(0xc4, (const uint8_t*)"\x80", 1); 

    // Display control / brightness 
    this->writeData(0x53, (const uint8_t*)"\x20", 1); 
    this->writeData(0x51, (const uint8_t*)"\xff", 1); 
    this->writeData(0x63, (const uint8_t*)"\xff", 1); 

    // Display on 
    this->writeData(0x29, nullptr, 0); 
    sleep_ms(50);
    this->clear();
    this->setBrightness(50);
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
            this->writeData(0x36, 0x00);
			this->height = height;
			this->width = width;
            this->maxWidth = LCD_WIDTH;
            this->maxHeight = LCD_HEIGHT;
            break;
        case 90:
			this->writeData(0x36, 0x20);
			this->height = width;
			this->width = height;
            this->maxWidth = LCD_HEIGHT;
            this->maxHeight = LCD_WIDTH;
            break;
        case 180:
			this->writeData(0x36, 0xc0);
			this->height = height;
			this->width = width;
            this->maxWidth = LCD_WIDTH;
            this->maxHeight = LCD_HEIGHT;
            break;
        case 270:
			this->writeData(0x36, 0x60);
			this->height = width;
			this->width = height;
            this->maxWidth = LCD_HEIGHT;
            this->maxHeight = LCD_WIDTH;
            break;
        default:
            return;
    }

    this->swap_offsets(rotation);
}

/**
 * @brief Turn the display on
*/
void cna3306::set_display_state(bool on)
{
    if(on) 
		this->writeData(0x29);
    else 
		this->writeData(0x28);
		
    sleep_ms(10);
}

/**
 * @brief Set the backlight brightness
 * @param brightness Brightness (0-100%)
 */
void cna3306::setBrightness(uint8_t brightness)
{
    uint32_t brightness32 = brightness;

    // convert the brightness from 0-100 to 0-255
    brightness32 = (brightness32 * 255) / 100;

    // apply a brightness curve to make the dimming more natural
    brightness32 = sqrt(brightness32 * 255);

    this->setBrightnessRaw(brightness32);
}

void cna3306::setBrightnessRaw(uint8_t brightness)
{
#if defined(LCD_BACKLIGHT_INV)
    brightness = 255 - brightness;
#endif
    this->writeData(COMMAND_BRIGHTNESS, brightness);
    this->brightness = brightness;
}

uint8_t cna3306::getBrightness(void)
{
    uint32_t brightness32 = this->brightness;
#if defined(LCD_BACKLIGHT_INV)
    brightness32 = 255 - brightness32;
#endif
    // convert brightness from 0-255 to 0-100
    brightness32 = (brightness32 * 100) / 255;

    // convert the brightness curve back to linear
    brightness32 = sqrt(brightness32 * 100);

    return brightness32;
}

uint8_t cna3306::getBrightnessRaw(void)
{
    return this->brightness;
}
#endif