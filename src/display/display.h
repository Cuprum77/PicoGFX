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
#if defined(LCD_EINK_DRIVER)
    display_obj(hardware_driver *hw, color_t *frameBuffer);
#else
    display_obj(hardware_driver *hw, color_t *frameBuffer, 
        uint32_t *CASET, uint32_t *RASET, uint32_t *RAMWR);
#endif
    virtual uint32_t getRotation(void) { return this->rotation; }
    virtual void clear(void);

#if defined(LCD_BACKLIGHT_ENABLED)
    virtual void initBacklight();
#if defined(LCD_BACKLIGHT_DIMMABLE)
    virtual void setBrightness(uint8_t brightness);
    virtual void setBrightnessRaw(uint8_t brightness);
    virtual uint8_t getBrightness(void);
    virtual uint8_t getBrightnessRaw(void);
#else
    virtual void setBrightness(bool on);
    virtual bool getBrightness(void);
#endif
#endif

    virtual void update();
    virtual void update(int32_t start, int32_t end);
    virtual void update(int32_t start, int32_t end, bool moveCursor);
    virtual void update(point start, point end);
    virtual void update(rect rect);
    
    virtual void frameCounter(void);
    virtual bool frameLimiter(uint32_t frameRate);

    virtual void setPixel(point point, color color);
    virtual void setPixel(uint32_t point, color_t color);
    virtual color getPixel(point point);
    virtual color_t getPixel(uint32_t index);

    virtual void setCursor(point point);
    virtual point getCursor(void);
    virtual point getCenter(void);

    virtual int32_t getFrameCounter() { return this->frames; }
    virtual uint32_t getWidth(void) { return this->width; }
    virtual uint32_t getHeight(void)  { return this->height; }
    virtual uint32_t getShortestSide(void) { return imin(this->width, this->height); }
    virtual uint32_t getLongestSide(void) { return imax(this->width, this->height); }
    virtual rect getArea(void) { return rect(point(0, 0), point(this->width, this->height)); }
    virtual color_t *getFrameBuffer(void) { return this->frameBuffer; }
    
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

    uint32_t *CASET;
    uint32_t *RASET;
    uint32_t *RAMWR;
    bool isEINK = false;

    // timer for the framerate calculation
    int32_t framecounter = 0;
    int32_t frames = 0;
    uint64_t timer = 0;
    uint64_t lastFrame = 0;

    virtual void writeData(uint8_t command, const uint8_t *data, size_t length);
    virtual void writeData(uint8_t command, uint8_t data) { writeData(command, &data, 1); }
    virtual void writeData(uint8_t command) { writeData(command, nullptr, 0); }
    virtual void writePixels(const color_t *data, size_t length);
    virtual void switchTransmissionMode(bool data) { this->hw->switchTransmissionMode(data); }
    virtual void swap_offsets(uint32_t rotation);
    virtual inline void columnAddressSet(uint32_t x0, uint32_t x1);
    virtual inline void rowAddressSet(uint32_t y0, uint32_t y1);
};
