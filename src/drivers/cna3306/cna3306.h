#pragma once

#include "lcd_config.h"

#if defined(LCD_DRIVER_CNA3306)
#include "display.h"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b
#define MAX_WIDTH 240
#define MAX_HEIGHT 360

#define FRAMEBUFFER_SIZE MAX_WIDTH * MAX_HEIGHT

class cna3306 : public display_obj
{
public:
    cna3306(hardware_driver *hw) : 
        display_obj(hw, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {}
    void init();

    void set_rotation(uint32_t rotation);
    void set_display_state(bool on);

private:
#if defined(LCD_COLOR_DEPTH_16)
    uint16_t framebuffer[FRAMEBUFFER_SIZE];
#elif defined(LCD_COLOR_DEPTH_18)
    uint32_t framebuffer[FRAMEBUFFER_SIZE];
#elif defined(LCD_COLOR_DEPTH_24)
    uint32_t framebuffer[FRAMEBUFFER_SIZE];
#else
#error "Unsupported color depth for CNA3306"
#endif
};
#endif