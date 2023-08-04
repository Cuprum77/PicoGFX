#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "FontStruct.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// String behavior
#define CHARACTER_BUFFER_SIZE 128 // max number of characters that can be printed at once
#define TAB_SIZE 4      // how many spaces a tab is worth
#define FALSE "false"   // string representation of false
#define TRUE "true"     // string representation of true

// Typedefs for number bases
typedef enum
{
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16
} number_base_t;

class Print
{
public:
    Print(unsigned short* frameBuffer, Display_Params params);
    void setColor(Color color);
    Color getColor(void);
    void setCursor(Point point);
    void moveCursor(int x, int y);
    Point getCursor(void);
    void setFont(FontStruct* font);

    void print(const char* format, ...);
    unsigned int getStringWidth(const char* format, ...);
    unsigned int getStringHeight(const char* format, ...);
private:
    unsigned short* frameBuffer;
    char characterBuffer[CHARACTER_BUFFER_SIZE];
    unsigned int width;
    unsigned int height;
    size_t totalPixels;
    unsigned long cursor;
    unsigned short color;
    FontStruct* font;

    void drawAscii(const char c);
    void floatToString(double num, char* buffer, unsigned int precision);
    void reverse(char* str, unsigned int length);
};