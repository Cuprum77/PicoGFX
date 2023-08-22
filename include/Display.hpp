#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/divider.h"
#include "hardware/pwm.h"

#include "Driver.hpp"
#include "Structs.h"
#include "Shapes.h"
#include "Color.h"


class Display
{
public:
    Display(Driver* spi, display_config_t* config, unsigned short* frameBuffer, unsigned char CASET, unsigned char RASET, unsigned char RAMWR);
    void setBrightness(unsigned char brightness);
    int getRotation(void) { return this->config->rotation; }
    void clear(void);

    void update(bool framecounter = false);
    void update(int start, int end);
    void update(int start, int end, bool moveCursor);
    void setPixel(Point point, Color color);
    void setPixel(unsigned int point, unsigned short color);
    Color getPixel(Point point);
    unsigned short getPixel(unsigned int point);

    void setCursor(Point point);
    Point getCursor(void);
    Point getCenter(void);

    int getFrameCounter() { return this->frames; }
    unsigned int getWidth(void) { return this->config->width; }
    unsigned int getHeight(void)  { return this->config->height; }
    unsigned short* getFrameBuffer(void) { return this->frameBuffer; }

protected:
    Driver* spi;
    display_config_t* config;
    bool dimmingEnabled = false;
    unsigned int sliceNum;
    unsigned int pwmChannel;
    bool dataMode = false;
    unsigned short* frameBuffer;
    Point cursor = {0, 0};
    bool backlight;
    unsigned int totalPixels;
    unsigned int maxWidth;
    unsigned int maxHeight;
    int CASET;
    int RASET;
    int RAMWR;

    // timer for the framerate calculation
    int framecounter = 0;
    int frames = 0;
    unsigned long timer = 0;

    void writeData(unsigned char command, const unsigned char* data, size_t length);
    void writeData(unsigned char command, unsigned char data) { writeData(command, &data, 1); }
    void writeData(unsigned char command) { writeData(command, nullptr, 0); }
    inline void columnAddressSet(unsigned int x0, unsigned int x1);
    inline void rowAddressSet(unsigned int y0, unsigned int y1);
    void writePixels(const unsigned short* data, size_t length);
};