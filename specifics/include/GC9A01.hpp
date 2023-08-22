#pragma once

#include "Display.hpp"

// Driver constants
#define COMMAND_RAMWR 0x2c
#define COMMAND_CASET 0x2a
#define COMMAND_RASET 0x2b
#define MAX_WIDTH 240
#define MAX_HEIGHT 240

#define FRAMEBUFFER_SIZE MAX_WIDTH * MAX_HEIGHT

class GC9A01 : public Display
{
public:
    GC9A01(Driver* spi, display_config_t* config) : 
        Display(spi, config, this->framebuffer, COMMAND_CASET, COMMAND_RASET, COMMAND_RAMWR) {} // Constructor
    void init();
    void reset();
    void softReset();

    void setRotation(int rotation);
    void setDisplayState(bool on);

private:
    unsigned short framebuffer[FRAMEBUFFER_SIZE];
};