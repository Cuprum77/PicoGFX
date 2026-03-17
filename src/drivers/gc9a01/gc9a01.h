#pragma once
#include "lcd_config.h"

#if defined(LCD_DRIVER_GC9A01)
#include "display.h"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b

class gc9a01 : public display_obj
{
public:
    gc9a01(hardware_driver *hw) : 
        display_obj(hw, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {}
    void init();
    void softReset();

    void set_rotation(uint32_t rotation);
    void set_display_state(bool on);

private:
    color_t framebuffer[LCD_FRAMEBUFFER_SIZE];
};
#endif