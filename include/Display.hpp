#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/divider.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"

#include "HardwareSPI.hpp"
#include "Commands.hpp"
#include "Structs.hpp"
#include "Color.hpp"
#include "Enums.hpp"

// Typedefs
#define ulong unsigned long
#define uint unsigned int
#define ushort unsigned short
#define uchar unsigned char

// Constants
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define sq(x) ((x) * (x))
// Limit of the ST7789 driver
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320
#define FRAMEBUFFER_SIZE (ST7789_WIDTH * ST7789_HEIGHT)

typedef enum
{
    TopLeft = 0,
    TopRight = 1,
    BottomLeft = 2,
    BottomRight = 3,
} displayCorner_t;

typedef enum
{
    DEG_0 = 0,
    DEG_90 = 1,
    DEG_180 = 2,
    DEG_270 = 3
} displayRotation_t;

class Display
{
public:
    Display(HardwareSPI* spi, Display_Pins* pins, Display_Params* params);
    bool writeReady(void) { return !this->spi->dma_busy(); }
    void setBrightness(unsigned char brightness);
    void setRotation(displayRotation_t rotation);
    void displayOn(void);
    void displayOff(void);
    void clear(void);
    void fill(Color color);
    Color getFillColor(void);

    void update(void);
    void setPixel(Point point, Color color);
    void setPixel(uint point, ushort color);
    Color getPixel(Point point);
    ushort getPixel(uint point);
    size_t getBufferSize(void);

    void setCursor(Point point);
    Point getCursor(void);
    Point getCenter(void);

    uint getWidth(void) { return this->params->width; }
    uint getHeight(void)  { return this->params->height; }
    ushort* getFrameBuffer(void) { return this->frameBuffer; }

protected:
    HardwareSPI* spi;
    Display_Pins* pins;
    Display_Params* params;
    bool dimmingEnabled = false;
    uint sliceNum;
    uint pwmChannel;
    bool dataMode = false;
    ushort frameBuffer[FRAMEBUFFER_SIZE + 1];
    Color fillColor;
    Point cursor = {0, 0};
    bool backlight;
    display_type_t type;
    bool BGR = false;
    uint maxWidth;
    uint maxHeight;
    uint totalPixels;

    void ST7789_Init(void);
    void ST7789_SetRotation(displayRotation_t rotation);

    void GC9A01_Init(void);
    void GC9A01_SoftReset(void);
    void GC9A01_HardReset(void);
    void GC9A01_SetRotation(displayRotation_t rotation);

    void writeData(Display_Commands command, 
        const unsigned char* data, size_t length) { writeData((uchar)command, data, length); }
    void writeData(Display_Commands command, uchar data) { writeData((uchar)command, &data, 1); }
    void writeData(Display_Commands command) { writeData(command, nullptr, 0); }
    void writeData(uchar command, const uchar* data, size_t length);
    void writeData(uchar command, uchar data) { writeData(command, &data, 1); }
    void writeData(uchar command) { writeData(command, nullptr, 0); }
    inline void columnAddressSet(uint x0, uint x1);
    inline void rowAddressSet(uint y0, uint y1);
    void writePixels(const unsigned short* data, size_t length);
};