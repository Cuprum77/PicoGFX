#pragma once
#include "lcd_config.h"

#if defined(LCD_DRIVER_ST7789)
#include "display.h"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b
#define MAX_WIDTH 240
#define MAX_HEIGHT 320

#define FRAMEBUFFER_SIZE MAX_WIDTH * MAX_HEIGHT

class st7789 : public display_obj
{
public:
    st7789(hardware_driver *hw) : 
        display_obj(hw, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {}
    void init();

    void set_rotation(uint32_t rotation);
    void set_display_state(bool on);

private:
    color_t framebuffer[FRAMEBUFFER_SIZE];
};
#endif