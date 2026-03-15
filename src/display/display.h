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
    display_obj(hardware_driver *hw, color_t *frameBuffer, uint8_t CASET, uint8_t RASET, uint8_t RAMWR);
    uint32_t getRotation(void) { return this->rotation; }
    void clear(void);

#if defined(LCD_BACKLIGHT_ENABLED)
    void initBacklight();
#if defined(LCD_BACKLIGHT_DIMMABLE)
    void setBrightness(uint8_t brightness);
    void setBrightnessRaw(uint8_t brightness);
    uint8_t getBrightness(void);
    uint8_t getBrightnessRaw(void);
#else
    void setBrightness(bool on);
    bool getBrightness(void);
#endif
#endif

    void update();
    void update(int32_t start, int32_t end);
    void update(int32_t start, int32_t end, bool moveCursor);
    void update(point start, point end);
    void update(rect rect);
    
    void frameCounter(void);
    bool frameLimiter(uint32_t frameRate);

    void setPixel(point point, color color);
    void setPixel(uint32_t point, color_t color);
    color getPixel(point point);
    color_t getPixel(uint32_t index);

    void setCursor(point point);
    point getCursor(void);
    point getCenter(void);

    int32_t getFrameCounter() { return this->frames; }
    uint32_t getWidth(void) { return this->width; }
    uint32_t getHeight(void)  { return this->height; }
    uint32_t getShortestSide(void) { return imin(this->width, this->height); }
    uint32_t getLongestSide(void) { return imax(this->width, this->height); }
    rect getArea(void) { return rect(point(0, 0), point(this->width, this->height)); }
    color_t *getFrameBuffer(void) { return this->frameBuffer; }
    
protected:
    hardware_driver *hw;
    color_t *frameBuffer;

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
    uint32_t offset_x0 = LCD_OFFSET_X0;
    uint32_t base_offset_x0 = LCD_OFFSET_X0;
#else
    uint32_t offset_x0 = 0;
    uint32_t base_offset_x0 = 0;
#endif

#if defined(LCD_OFFSET_X1)
    uint32_t offset_x1 = LCD_OFFSET_X1;
    uint32_t base_offset_x1 = LCD_OFFSET_X1;
#else
    uint32_t offset_x1 = 0;
    uint32_t base_offset_x1 = 0;
#endif

#if defined(LCD_OFFSET_Y0)
    uint32_t offset_y0 = LCD_OFFSET_Y0;
    uint32_t base_offset_y0 = LCD_OFFSET_Y0;
#else
    uint32_t offset_y0 = 0;
    uint32_t base_offset_y0 = 0;
#endif

#if defined(LCD_OFFSET_Y1)
    uint32_t offset_y1 = LCD_OFFSET_Y1;
    uint32_t base_offset_y1 = LCD_OFFSET_Y1;
#else
    uint32_t offset_y1 = 0;
    uint32_t base_offset_y1 = 0;
#endif

#if defined(LCD_BACKLIGHT_ENABLED)
#if defined(LCD_BACKLIGHT_DIMMABLE)
    uint32_t sliceNum;
    uint32_t pwmChannel;
    uint8_t brightness;
#else
    bool backlight;
#endif
#endif

    bool dataMode = false;
    point cursor = {0, 0};
    uint32_t totalPixels;

    uint32_t width = LCD_WIDTH;
    uint32_t maxWidth;
    uint32_t height = LCD_HEIGHT;
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
    void writePixels(const color_t *data, size_t length);
    void switchTransmissionMode(bool data) { this->hw->switchTransmissionMode(data); }
    void swap_offsets(uint32_t rotation);
    inline void columnAddressSet(uint32_t x0, uint32_t x1);
    inline void rowAddressSet(uint32_t y0, uint32_t y1);
};
