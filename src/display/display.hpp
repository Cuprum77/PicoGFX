#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/divider.h"
#include "hardware/pwm.h"
#include "hardware_driver.hpp"
#include "display_struct.h"
#include "shapes.hpp"
#include "color.h"
#include "gfxmath.h"


class display
{
public:
    display(hardware_driver* hw, display_config_t* config, uint16_t* frameBuffer, uint8_t CASET, uint8_t RASET, uint8_t RAMWR);
    void setBrightness(uint8_t brightness);
    display_rotation_t getRotation(void) { return this->config->rotation; }
    void clear(void);

    void update();
    void update(int32_t start, int32_t end);
    void update(int32_t start, int32_t end, bool moveCursor);
    void update(point start, point end);
    void update(rect rect);
    
    void frameCounter(void);
    bool frameLimiter(uint32_t frameRate);

    void setPixel(point point, color color);
    void setPixel(uint32_t point, uint16_t color);
    color getPixel(point point);
    uint16_t getPixel(uint32_t point);

    void setCursor(point point);
    point getCursor(void);
    point getCenter(void);

    int32_t getFrameCounter() { return this->frames; }
    uint32_t getWidth(void) { return this->config->width; }
    uint32_t getHeight(void)  { return this->config->height; }
    uint32_t getShortestSide(void) { return imin(this->config->width, this->config->height); }
    uint32_t getLongestSide(void) { return imax(this->config->width, this->config->height); }
    rect getArea(void) { return rect(point(0, 0), point(this->config->width, this->config->height)); }

    uint16_t* getFrameBuffer(void) { return this->frameBuffer; }

protected:
    hardware_driver* hw;
    display_config_t* config;
    bool dimmingEnabled = false;
    uint32_t sliceNum;
    uint32_t pwmChannel;
    bool dataMode = false;
    uint16_t* frameBuffer;
    point cursor = {0, 0};
    bool backlight;
    uint32_t totalPixels;
    uint32_t maxWidth;
    uint32_t maxHeight;
    int32_t CASET;
    int32_t RASET;
    int32_t RAMWR;

    // timer for the framerate calculation
    int32_t framecounter = 0;
    int32_t frames = 0;
    uint64_t timer = 0;
    uint64_t lastFrame = 0;

    void writeData(uint8_t command, const uint8_t* data, size_t length);
    void writeData(uint8_t command, uint8_t data) { writeData(command, &data, 1); }
    void writeData(uint8_t command) { writeData(command, nullptr, 0); }
    inline void columnAddressSet(uint32_t x0, uint32_t x1);
    inline void rowAddressSet(uint32_t y0, uint32_t y1);
    void writePixels(const uint16_t* data, size_t length);
};
