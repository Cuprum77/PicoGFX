#pragma once
#include "lcd_config.h"

#if defined(LCD_DRIVER_UC8151)
#include "display.h"

// Driver constants
#define COMMAND_MEMB1   0x10
#define COMMAND_MEMB2   0x13
#define MAX_WIDTH       128
#define MAX_HEIGHT      296

#define FRAMEBUFFER_SIZE MAX_WIDTH * MAX_HEIGHT

class uc8151 : public display_obj
{
public:
    uc8151(hardware_driver *hw) : 
        display_obj(hw, this->framebuffer) {}
    void init();
    void refresh();

    void set_rotation(uint32_t rotation);
    void set_display_state(bool on);

private:
    color_t framebuffer[FRAMEBUFFER_SIZE];
    color_t secound_framebuffer[FRAMEBUFFER_SIZE];

    void writePixels(const color_t *data, size_t length) override;
};
#endif