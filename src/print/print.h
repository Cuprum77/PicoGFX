#pragma once
#include "lcd_config.h"
#if defined(PICO_GFX_PRINT)

#include "color.h"
#include "shapes.h"
#include "pico_gfx_fonts.h"
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
    printer(void *frameBuffer, display_obj *display_ptr = nullptr);

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
#if defined(LCD_COLOR_DEPTH_1)
    bool *frameBuffer;
#elif defined(LCD_COLOR_DEPTH_8)
    uint8_t *frameBuffer;
#elif defined(LCD_COLOR_DEPTH_16)
    uint16_t *frameBuffer;
#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
    uint32_t *frameBuffer;
#else
#error "Unsupported color depth"
#endif
    display_obj *display_ptr;

    // print variables
    char characterBuffer[CHARACTER_BUFFER_SIZE];
    int32_t charactersInBuffer = 0;
    uint32_t cursor;
    uint16_t color_val;
    FontStruct *font;

    // Private helper functions
    int32_t getStringYOffset();
    void drawAscii(const char c);
};
#endif