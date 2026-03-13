#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/divider.h"
#include "hardware/pwm.h"
#include "driver.h"
#include "lcd_config.h"
#include "shapes.h"
#include "color.h"
#include "gfxmath.h"

class display_obj
{
public:
    display_obj(hardware_driver *hw, void *frameBuffer, uint8_t CASET, uint8_t RASET, uint8_t RAMWR);
    uint32_t getRotation(void) { return this->rotation; }
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

    void setCursor(point point);
    point getCursor(void);
    point getCenter(void);

    int32_t getFrameCounter() { return this->frames; }
    uint32_t getWidth(void) { return this->width; }
    uint32_t getHeight(void)  { return this->height; }
    uint32_t getShortestSide(void) { return imin(this->width, this->height); }
    uint32_t getLongestSide(void) { return imax(this->width, this->height); }
    rect getArea(void) { return rect(point(0, 0), point(this->width, this->height)); }

#if defined(LCD_COLOR_DEPTH_1)
    bool getPixel(uint32_t index);
    bool *getFrameBuffer(void) { return this->frameBuffer; }
#elif defined(LCD_COLOR_DEPTH_8)
    uint8_t getPixel(uint32_t index);
    uint8_t *getFrameBuffer(void) { return this->frameBuffer; }
#elif defined(LCD_COLOR_DEPTH_16)
    uint16_t getPixel(uint32_t index);
    uint16_t *getFrameBuffer(void) { return this->frameBuffer; }
#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
    uint32_t getPixel(uint32_t index);
    uint32_t *getFrameBuffer(void) { return this->frameBuffer; }
#else
#error "Unsupported color depth"
#endif
    
protected:
    hardware_driver *hw;

#if defined(LCD_ROTATION_0)
    uint32_t rotation = 0;
#elif defined(LCD_ROTATION_90)
    uint32_t rotation = 90;
#elif defined(LCD_ROTATION_180)
    uint32_t rotation = 180;
#elif defined(LCD_ROTATION_270)
    uint32_t rotation = 270;
#else
    uint32_t rotation = 0;
#endif

#if defined(LCD_OFFSET_X0)
    uint32_t columnOffset1 = LCD_OFFSET_X0;
#else
    uint32_t columnOffset1 = 0;
#endif

#if defined(LCD_OFFSET_X1)
    uint32_t columnOffset2 = LCD_OFFSET_X1;
#else
    uint32_t columnOffset2 = 0;
#endif

#if defined(LCD_OFFSET_Y0)
    uint32_t rowOffset1 = LCD_OFFSET_Y0;
#else
    uint32_t rowOffset1 = 0;
#endif

#if defined(LCD_OFFSET_Y1)
    uint32_t rowOffset2 = LCD_OFFSET_Y1;
#else
    uint32_t rowOffset2 = 0;
#endif

    uint32_t sliceNum;
    uint32_t pwmChannel;
    bool dataMode = false;

#if defined(LCD_COLOR_DEPTH_1)
    bool *frameBuffer;
    void writePixels(const bool *data, size_t length);
#elif defined(LCD_COLOR_DEPTH_8)
    uint8_t *frameBuffer;
    void writePixels(const uint8_t *data, size_t length);
#elif defined(LCD_COLOR_DEPTH_16)
    uint16_t *frameBuffer;
    void writePixels(const uint16_t *data, size_t length);
#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
    uint32_t *frameBuffer;
    void writePixels(const uint32_t *data, size_t length);
#else
#error "Unsupported color depth"
#endif

    point cursor = {0, 0};
    bool backlight;
    uint32_t totalPixels;

    uint32_t width;
    uint32_t maxWidth;
    uint32_t height;
    uint32_t maxHeight;

    int32_t CASET;
    int32_t RASET;
    int32_t RAMWR;

    // timer for the framerate calculation
    int32_t framecounter = 0;
    int32_t frames = 0;
    uint64_t timer = 0;
    uint64_t lastFrame = 0;

    void writeData(uint8_t command, const uint8_t *data, size_t length);
    void writeData(uint8_t command, uint8_t data) { writeData(command, &data, 1); }
    void writeData(uint8_t command) { writeData(command, nullptr, 0); }
    inline void columnAddressSet(uint32_t x0, uint32_t x1);
    inline void rowAddressSet(uint32_t y0, uint32_t y1);
};
