#pragma once
#include "lcd_config.h"

#if defined(LCD_DRIVER_ST7262)
#include "display.h"

#if defined(LCD_DRIVER_ST7262) && !defined(LCD_HARDWARE_RGB) 
#error "Driver does not support non-RGB interfaces"
#endif

class st7262 : public display_obj
{
public:
    st7262(hardware_driver *hw) : 
        display_obj(hw, this->framebuffer) {}
    void init();
    void set_rotation(uint32_t rotation);

private:
    color_t framebuffer[LCD_FRAMEBUFFER_SIZE];
};
#endif