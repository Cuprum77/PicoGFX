#pragma once

#include "lcd_config.h"

#if defined(LCD_DRIVER_CNA3306)
#include "display.h"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b
#define COMMAND_BRIGHTNESS 0x51

class cna3306 : public display_obj
{
public:
    cna3306(hardware_driver *hw) : 
        display_obj(hw, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {}
    void init();

    void set_rotation(uint32_t rotation);
    void set_display_state(bool on);
    void setBrightness(uint8_t brightness);
    void setBrightnessRaw(uint8_t brightness);
    uint8_t getBrightness(void);
    uint8_t getBrightnessRaw(void);

private:
    color_t framebuffer[LCD_FRAMEBUFFER_SIZE];
    uint8_t brightness = 0;
};
#endif