#pragma once
#include "lcd_config.h"

#if defined(LCD_DRIVER_GC9A01)
#include "display.h"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b
#define MAX_WIDTH 240
#define MAX_HEIGHT 240

#define FRAMEBUFFER_SIZE MAX_WIDTH * MAX_HEIGHT

class gc9a01 : public display
{
public:
    gc9a01(hardware_driver *hw) : 
        display(hw, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {}
    void init();
    void softReset();

    void setRotation(uint32_t rotation);
    void setDisplayState(bool on);

private:
#if defined(LCD_COLOR_DEPTH_16)
    uint16_t framebuffer[FRAMEBUFFER_SIZE];
#elif defined(LCD_COLOR_DEPTH_18)
    uint32_t framebuffer[FRAMEBUFFER_SIZE];
#else
#error "Unsupported color depth for GC9A01"
#endif
};
#endif