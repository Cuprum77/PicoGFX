#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "FontStruct.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

// String behavior
#define CHARACTER_BUFFER_SIZE 256 // max number of characters that can be printed at once
#define TAB_SIZE 4  // how many spaces a tab is worth

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
    // Display variables
    unsigned short* frameBuffer;
    unsigned int width;
    unsigned int height;
    size_t totalPixels;

    // Print variables
    char characterBuffer[CHARACTER_BUFFER_SIZE];
    unsigned long cursor;
    unsigned short color;
    FontStruct* font;

    void drawAscii(const char c);
};