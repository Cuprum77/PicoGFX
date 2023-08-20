#include "Display.hpp"

void Display::ST7789_Init()
{
    // constants for this driver
    this->BGR = false;
    this->maxHeight = 320;
    this->maxWidth = 240;
    
    // reset the display
    this->writeData(Display_Commands::SWRESET, NULL, 0);
    sleep_ms(100);

    // set the display to sleep out
    this->writeData(Display_Commands::SLPOUT, NULL, 0);
    sleep_ms(50);

    // set the display to interface pixel format
    // 0x5 << 4 = 65k of rgb interface
    // 0x5 = 16 bits per pixel
    unsigned char pixelFormat = 0x5 << 4 | 0x5;
    this->writeData(Display_Commands::COLMOD, &pixelFormat, 1);
    sleep_ms(10);

    // madctl = memory access control
    this->setRotation(this->params->rotation);

    // set the display to memory access control
    this->setCursor({0, 0});

    // display inversion on
    this->writeData(Display_Commands::INVON, NULL, 0);
    sleep_ms(10);

    // normal display mode on
    this->writeData(Display_Commands::NORON, NULL, 0);
    sleep_ms(10);
}

void Display::ST7789_SetRotation(int rotation)
{
    rotation %= 4; // Normalize rotation to 0-3

	switch(rotation)
    {
        case 0:
            this->writeData(MADCTL, (unsigned char)Display_MADCTL::RGB);
            break;
        case 1:
			this->writeData(MADCTL, (unsigned char)(Display_MADCTL::MX | Display_MADCTL::MV | Display_MADCTL::RGB));
            break;
        case 2:
			this->writeData(MADCTL, (unsigned char)(Display_MADCTL::MX | Display_MADCTL::MY | Display_MADCTL::RGB));
            break;
        case 3:
			this->writeData(MADCTL, (unsigned char)(Display_MADCTL::MV | Display_MADCTL::MY | Display_MADCTL::RGB));
            break;
    }
}