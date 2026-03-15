#pragma once

#include "display.hpp"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b
#define MAX_WIDTH 240
#define MAX_HEIGHT 320

#define FRAMEBUFFER_SIZE MAX_WIDTH * MAX_HEIGHT

class st7789 : public display
{
public:
    st7789(hardware_driver* hw, display_config_t* config) : 
        display(hw, config, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {} // Constructor
    void init();

    void setRotation(display_rotation_t rotation);
    void setDisplayState(bool on);

private:
    uint16_t framebuffer[FRAMEBUFFER_SIZE];
};