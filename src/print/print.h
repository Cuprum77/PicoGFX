#pragma once

#include "color.h"
#include "shapes.h"
#include "fontstruct.h"
#include "display.h"

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

class printer 
{
public:
    // Constructor
    printer(uint16_t *frameBuffer, display *display_ptr = nullptr);

    // Configuration functions
    void setColor(color val);
    color getColor(void);
    void setCursor(point val);
    void moveCursor(int32_t x, int32_t y);
    point getCursor(void);
    void setFont(FontStruct *font);

    // print functions with helper functions
    void setString(const char *format, ...);
    void center(Alignment_t alignment = Alignment_t::TotalCenter);
    void print();
    uint32_t getStringWidth();
    uint32_t getStringHeight();

    // print function without helper functions
    void print(const char *format, ...);
private:
    // Display variables
    uint16_t *frameBuffer;
    display *display_ptr;

    // print variables
    char characterBuffer[CHARACTER_BUFFER_SIZE];
    int32_t charactersInBuffer = 0;
    uint32_t cursor;
    uint16_t color_val;
    FontStruct *font;

    // Private helper functions
    void drawAscii(const char c);
};