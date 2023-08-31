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

typedef enum
{
	HorizontalCenter,
	VerticalCenter,
	TotalCenter
} Alignment_t;

class Print
{
public:
    // Constructor
    Print(unsigned short* frameBuffer, display_config_t* config);

    // Configuration functions
    void setColor(Color color);
    Color getColor(void);
    void setCursor(Point point);
    void moveCursor(int x, int y);
    Point getCursor(void);
    void setFont(FontStruct* font);

    // Print functions with helper functions
    void setString(const char* format, ...);
    void center(Alignment_t alignment = Alignment_t::TotalCenter);
    void print();
    unsigned int getStringWidth();
    unsigned int getStringHeight();

    // Print function without helper functions
    void print(const char* format, ...);
private:
    // Display variables
    unsigned short* frameBuffer;
    display_config_t* config;

    // Print variables
    char characterBuffer[CHARACTER_BUFFER_SIZE];
    int charactersInBuffer = 0;
    unsigned long cursor;
    unsigned short color;
    FontStruct* font;

    // Private helper functions
    void drawAscii(const char c);
};