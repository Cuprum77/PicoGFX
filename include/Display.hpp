#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "Commands.hpp"
#include "Structs.hpp"
#include "Color.hpp"
#include "Enums.hpp"

#include "Font.h"


// Typedefs
#define ulong unsigned long
#define uint unsigned int
#define ushort unsigned short
#define uchar unsigned char

// Typedefs for number bases
#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16

// SPI
#define SPI_BAUDRATE 125000000  // 125 MHz

// Constants
#define CHARACTER_BUFFER_SIZE 128

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define sq(x) ((x) * (x))
// Limit of the ST7789 driver
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320
#define FRAMEBUFFER_SIZE (ST7789_WIDTH * ST7789_HEIGHT)
// String behavior
#define TAB_SIZE 4      // how many spaces a tab is worth
#define FALSE "false"   // string representation of false
#define TRUE "true"     // string representation of true

enum display_type_t
{
    ST7789,
    GC9A01
};

class Display
{
public:
    Display(spi_inst_t* spi, Display_Pins pins, 
        Display_Params params, display_type_t type = ST7789, bool dimming = false);
    void clear(void);
    void fill(Color color);
    Color getFillColor(void);
    void drawPixel(Point point, Color color);
    void displayOn(void);
    void displayOff(void);
    void setCursor(Point point);
    Point getCursor(void);
    Point getCenter(void);
    void setRotation(displayRotation_t rotation);
    void setBrightness(unsigned char brightness);
    void setTearing(bool enable);

    void fillGradient(Color startColor, Color endColor, Point start, Point end, uint steps = 256);
    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rectangle rect, Color color = Colors::White);
    void drawRectangle(Point center, uint width, uint height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);
    void drawCircle(Point center, uint radius, Color color = Colors::White);
    void drawFilledCircle(Point center, uint radius, Color color = Colors::White);
    void drawArc(Point center, uint radius, uint start_angle, uint end_angle, Color color = Colors::White);
    void drawFilledArc(Point center, uint radius, uint start_angle, uint end_angle, uint outer_radius, uint inner_radius, Color color = Colors::White);

    void drawBitmap(const unsigned char* bitmap, uint width, uint height);
    void drawBitmap(const unsigned short* bitmap, uint width, uint height);

    void write(char c, uchar size = 1, uchar base = DEC);
    
    void print(char num, Color color, uchar size = 1, uchar base = DEC);
    void print(char num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(uchar c, uchar size = 1, uchar base = DEC);
    void print(uchar num, Color color, uchar size = 1, uchar base = DEC);
    void print(uchar num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(short num, uchar size = 1, uchar base = DEC);
    void print(short num, Color color, uchar size = 1, uchar base = DEC);
    void print(short num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(ushort num, uchar size = 1, uchar base = DEC);
    void print(ushort num, Color color, uchar size = 1, uchar base = DEC);
    void print(ushort num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(int num, uchar size = 1, uchar base = DEC);
    void print(int num, Color color, uchar size = 1, uchar base = DEC);
    void print(int num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(uint num, uchar size = 1, uchar base = DEC);
    void print(uint num, Color color, uchar size = 1, uchar base = DEC);
    void print(uint num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(long num, uchar size = 1, uchar base = DEC);
    void print(long num, Color color, uchar size = 1, uchar base = DEC);
    void print(long num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(ulong num, uchar size = 1, uchar base = DEC);
    void print(ulong num, Color color, uchar size = 1, uchar base = DEC);
    void print(ulong num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(double num, uint precision = 2, uchar size = 1);
    void print(double num, Color color, uint precision = 2, uchar size = 1);
    void print(double num, Color color, Color background, uint precision = 2, uchar size = 1);
    void print(const char* text, uchar size = 1);
    void print(const char* text, Color color, uchar size = 1);
    void print(const char* text, Color color, Color background, uchar size = 1);
    void print(bool value);
    
    void println(char c, uchar size = 1, uchar base = DEC);
    void println(char num, Color color, uchar size = 1, uchar base = DEC);
    void println(char num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(uchar c, uchar size = 1, uchar base = DEC);
    void println(uchar num, Color color, uchar size = 1, uchar base = DEC);
    void println(uchar num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(int num, uchar size = 1, uchar base = DEC);
    void println(int num, Color color, uchar size = 1, uchar base = DEC);
    void println(int num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(uint num, uchar size = 1, uchar base = DEC);
    void println(uint num, Color color, uchar size = 1, uchar base = DEC);
    void println(uint num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(short num, uchar size = 1, uchar base = DEC);
    void println(short num, Color color, uchar size = 1, uchar base = DEC);
    void println(short num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(ushort num, uchar size = 1, uchar base = DEC);
    void println(ushort num, Color color, uchar size = 1, uchar base = DEC);
    void println(ushort num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(long num, uchar size = 1, uchar base = DEC);
    void println(long num, Color color, uchar size = 1, uchar base = DEC);
    void println(long num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(ulong num, uchar size = 1, uchar base = DEC);
    void println(ulong num, Color color, uchar size = 1, uchar base = DEC);
    void println(ulong num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void println(double num, uint precision = 2, uchar size = 1);
    void println(double num, Color color, uint precision = 2, uchar size = 1);
    void println(double num, Color color, Color background, uint precision = 2, uchar size = 1);
    void println(const char* text, uchar size = 1);
    void println(const char* text, Color color, uchar size = 1);
    void println(const char* text, Color color, Color background, uchar size = 1);
    void println(bool value);
    void println(void);

    uint getStringLength(char num, uchar size = 1, uchar base = DEC);
    uint getStringLength(uchar num, uchar size = 1, uchar base = DEC);
    uint getStringLength(int num, uchar size = 1, uchar base = DEC);
    uint getStringLength(uint num, uchar size = 1, uchar base = DEC);
    uint getStringLength(short num, uchar size = 1, uchar base = DEC);
    uint getStringLength(ushort num, uchar size = 1, uchar base = DEC);
    uint getStringLength(long num, uchar size = 1, uchar base = DEC);
    uint getStringLength(ulong num, uchar size = 1, uchar base = DEC);
    uint getStringLength(double num, uchar precision = 2, uchar size = 1);
    uint getStringLength(const char* text, uchar size = 1);
    uint getStringLength(bool value, uchar size = 1);
protected:
    spi_inst_t* spi;
    Display_Pins pins;
    Display_Params params;
    bool dimmingEnabled = false;
    uint sliceNum;
    uint pwmChannel;
    bool dataMode = false;
    ushort frameBufferColumn[ST7789_WIDTH + 1] = {0};
    ushort frameBufferRow[ST7789_HEIGHT + 1] = {0};
    ushort frameBuffer[FRAMEBUFFER_SIZE + 1] = {0};
    Color fillColor;
    Point cursor = {0, 0};
    bool backlight;
    display_type_t type;
    bool BGR = false;
    uint maxWidth;
    uint maxHeight;

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
    void columnAddressSet(uint x0, uint x1);
    void rowAddressSet(uint y0, uint y1);
    void writePixels(const unsigned short* data, size_t length);
    void BGRtoRGB(unsigned short* color);
    void BGRtoRGB(unsigned short* color, size_t length);
    void interpolate(ushort *color, size_t len, Color start, Color end, uint steps);

    uint drawAscii(const char c, Point Point, uint size, Color color, Color background);
    void floatToString(double num, char* buffer, uint precision);
    void reverse(char* str, uint length);
};