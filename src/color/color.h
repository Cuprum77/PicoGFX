#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    
#include <stdint.h>
#include "lcd_config.h"

#define MAX_COLOR_DIFF 255

#if !defined(LCD_COLOR_DEPTH_1)
#define COLOR_BLACK          0x000000
#define COLOR_NAVY           0x000080
#define COLOR_DARK_GREEN     0x006400
#define COLOR_DARK_CYAN      0x008b8b
#define COLOR_MAROON         0x800000
#define COLOR_PURPLE         0x800080
#define COLOR_OLIVE          0x808000
#define COLOR_LIGHT_GREEN    0x90ee90
#define COLOR_DARK_GREY      0xa9a9a9
#define COLOR_BLUE           0x0000ff
#define COLOR_GREEN          0x00ff00
#define COLOR_CYAN           0x00ffff
#define COLOR_RED            0xff0000
#define COLOR_MAGENTA        0xff00ff
#define COLOR_YELLOW         0xffff00
#define COLOR_WHITE          0xffffff
#define COLOR_ORANGE         0xff961f
#define COLOR_GREEN_YELLOW   0xadff2f
#define COLOR_PINK           0xffa1ea
#define COLOR_CUM            0xf8f8ff
#define COLOR_RASPBERRY_RED  0xe30b5d
#define COLOR_DARK_ORANGE    0xff8c00
#define COLOR_GOLD           0xffd700
#define COLOR_PISS           0xffd100
#define COLOR_DERG           0x00f4ff
#define COLOR_ORANGE_RED     0xff4500
#define COLOR_DARK_YELLOW    0xcccc00
#define COLOR_ARGENT         0xc0c0c0
#define COLOR_ACID_GREEN     0xb0bf1a
#define COLOR_ISLAMIC_GREEN  0x009000
#define COLOR_TURQUOISE_SURF 0x00c5cd
#define COLOR_DEEP_MAGENTA   0xcc00cc
#define COLOR_UE_RED         0xee2c2c
#define COLOR_MEDIUM_BLUE    0x0000cd
#define COLOR_CHINESE_BLACK  0x141414
#define COLOR_OXFORD_BLUE    0x002147
#define COLOR_DEEP_VIOLET    0x330066
#define COLOR_VAMPIRE_BLACK  0x0f0404
#define COLOR_EERIE_BLACK    0x1b1b1b

#if defined(LCD_COLOR_DEPTH_8)
// RGB888 -> RGB332
#define ADJ_COLOR(c) (((c) & 0xE00000) >> 16 | \
                      ((c) & 0x00E000) >> 11 | \
                      ((c) & 0x0000C0) >> 6)

#elif defined(LCD_COLOR_DEPTH_16)
// RGB888 -> RGB565
#define ADJ_COLOR(c) (((c) & 0xF80000) >> 8 | \
                      ((c) & 0x00FC00) >> 5 | \
                      ((c) & 0x0000F8) >> 3)

#elif defined(LCD_COLOR_DEPTH_18)
// RGB888 -> RGB666
#define ADJ_COLOR(c) ((((c >> 18) & 0x3F) << 12) | \
                      (((c >> 10) & 0x3F) <<  6) | \
                      (((c >>  2) & 0x3F)))

#elif defined(LCD_COLOR_DEPTH_24)
#define ADJ_COLOR(c) (c)
#endif

#else
#define COLOR_WHITE 1
#define COLOR_BLACK 0
#endif

#if !defined(LCD_COLOR_DEPTH_1)
/**
 * @brief Colors enum
*/
typedef enum
{
    black =         ADJ_COLOR(COLOR_BLACK),
    navy =          ADJ_COLOR(COLOR_NAVY),
    darkGreen =     ADJ_COLOR(COLOR_DARK_GREEN),
    darkCyan =      ADJ_COLOR(COLOR_DARK_CYAN),
    maroon =        ADJ_COLOR(COLOR_MAROON),
    purple =        ADJ_COLOR(COLOR_PURPLE),
    olive =         ADJ_COLOR(COLOR_OLIVE),
    lightGreen =    ADJ_COLOR(COLOR_LIGHT_GREEN),
    darkGrey =      ADJ_COLOR(COLOR_DARK_GREY),
    blue =          ADJ_COLOR(COLOR_BLUE),
    green =         ADJ_COLOR(COLOR_GREEN),
    cyan =          ADJ_COLOR(COLOR_CYAN),
    red =           ADJ_COLOR(COLOR_RED),
    magenta =       ADJ_COLOR(COLOR_MAGENTA),
    yellow =        ADJ_COLOR(COLOR_YELLOW),
    white =         ADJ_COLOR(COLOR_WHITE),
    orange =        ADJ_COLOR(COLOR_ORANGE),
    greenYellow =   ADJ_COLOR(COLOR_GREEN_YELLOW),
    pink =          ADJ_COLOR(COLOR_PINK),
    cum =           ADJ_COLOR(COLOR_CUM),
    raspberryRed =  ADJ_COLOR(COLOR_RASPBERRY_RED),
    darkOrange =    ADJ_COLOR(COLOR_DARK_ORANGE),
    gold =          ADJ_COLOR(COLOR_GOLD),
    piss =          ADJ_COLOR(COLOR_PISS),
    derg =          ADJ_COLOR(COLOR_DERG),
    orangeRed =     ADJ_COLOR(COLOR_ORANGE_RED),
    darkYellow =    ADJ_COLOR(COLOR_DARK_YELLOW),
    argent =        ADJ_COLOR(COLOR_ARGENT),
    acidGreen =     ADJ_COLOR(COLOR_ACID_GREEN),
    islamicGreen =  ADJ_COLOR(COLOR_ISLAMIC_GREEN),
    turquoiseSurf = ADJ_COLOR(COLOR_TURQUOISE_SURF),
    deepMagenta =   ADJ_COLOR(COLOR_DEEP_MAGENTA),
    ueRed =         ADJ_COLOR(COLOR_UE_RED),
    mediumBlue =    ADJ_COLOR(COLOR_MEDIUM_BLUE),
    chineseBlack =  ADJ_COLOR(COLOR_CHINESE_BLACK),
    oxfordBlue =    ADJ_COLOR(COLOR_OXFORD_BLUE),
    deepViolet =    ADJ_COLOR(COLOR_DEEP_VIOLET),
    vampireBlack =  ADJ_COLOR(COLOR_VAMPIRE_BLACK),
    eerieBlack =    ADJ_COLOR(COLOR_EERIE_BLACK)
} colors;
#endif

struct color
{
#if defined(LCD_COLOR_DEPTH_1)
    bool white;
#elif defined(LCD_COLOR_DEPTH_8)
    uint8_t r : 3;
    uint8_t g : 3;
    uint8_t b : 2;
#elif defined(LCD_COLOR_DEPTH_16)
    uint16_t r : 5;
    uint16_t g : 6;
    uint16_t b : 5;
// Unless the color depth can take advantage of a smaller data type, theres no point in keeping them separated
#elif defined(LCD_COLOR_DEPTH_18)
    uint32_t   : 6; // padding to align to byte boundary
    uint32_t r : 6;
    uint32_t g : 6;
    uint32_t b : 6;
#elif defined(LCD_COLOR_DEPTH_24)
    uint32_t r : 8;
    uint32_t g : 8;
    uint32_t b : 8;
#endif

    /**
     * @brief Creates a black color
    */
    color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

#if !defined(LCD_COLOR_DEPTH_1)
    /**
     * @brief Creates a color from a Colors enum
     * @param color Colors enum
    */
    color(colors c)
    {
#if defined(LCD_COLOR_DEPTH_8)
        this->r = (c >> 5) & 0x03;
        this->g = (c >> 2) & 0x03;
        this->b = c & 0x02;
#elif defined(LCD_COLOR_DEPTH_16)
        this->r = (c >> 11) & 0x1f;
        this->g = (c >> 5) & 0x3f;
        this->b = c & 0x1f;
#elif defined(LCD_COLOR_DEPTH_18)
        this->r = (c >> 12) & 0x3f;
        this->g = (c >> 6) & 0x3f;
        this->b = c & 0x3f;
#elif defined(LCD_COLOR_DEPTH_24)
        this->r = (c >> 16) & 0xff;
        this->g = (c >> 8) & 0xff;
        this->b = c & 0xff;
#endif
    }
#endif

    /**
     * @brief Creates a color from a 8 bit value
     * @param color 8-bit color
    */
    color(uint8_t c)
    {
        this->r = (c >> 5) & 0x03;
        this->g = (c >> 2) & 0x03;
        this->b = c & 0x02;
    }

    /**
     * @brief Creates a color from a 16 bit value
     * @param color 16-bit color
    */
    color(uint16_t c)
    {
#if defined(LCD_COLOR_DEPTH_1)
        this->white = c >= 0x7fff;
#elif defined(LCD_COLOR_DEPTH_8)
        this->r = (c >> 5) & 0x07;
        this->g = (c >> 2) & 0x07;
        this->b = c & 0x03;
#elif defined(LCD_COLOR_DEPTH_16)
        this->r = (c >> 11) & 0x1f;
        this->g = (c >> 5) & 0x3f;
        this->b = c & 0x1f;
// We can't get more information out of nothing, so we just put the bits in the right place
#elif defined(LCD_COLOR_DEPTH_18)
        this->r = ((c >> 11) & 0x1f) << 1;
        this->g = ((c >> 5) & 0x3f);
        this->b = (c & 0x1f) << 1;
#elif defined(LCD_COLOR_DEPTH_24)
        this->r = ((c >> 11) & 0x1f) << 3;
        this->g = ((c >> 5) & 0x3f) << 2;
        this->b = (c & 0x1f) << 3;
#endif
    }

    /**
     * @brief Creates a color from a 24 bit value
     * @param color 16-bit color
    */
    color(uint32_t c)
    {
#if defined(LCD_COLOR_DEPTH_1)
        if (c >= 0x7fffff)
            this->white = true;
        else
            this->white = false;
#elif defined(LCD_COLOR_DEPTH_8)
        uint8_t r = (c >> 16) & 0xff;
        uint8_t g = (c >> 8) & 0xff;
        uint8_t b = c & 0xff;

        this->r = r >> 5 & 0x07;
        this->g = g >> 5 & 0x07;
        this->b = b >> 6 & 0x03;
#elif defined(LCD_COLOR_DEPTH_16)
        uint8_t r = (c >> 16) & 0xff;
        uint8_t g = (c >> 8) & 0xff;
        uint8_t b = c & 0xff;

        this->r = r >> 3 & 0x1f;
        this->g = g >> 2 & 0x3f;
        this->b = b >> 3 & 0x1f;
#elif defined(LCD_COLOR_DEPTH_18)
        uint8_t r = (c >> 16) & 0xff;
        uint8_t g = (c >> 8) & 0xff;
        uint8_t b = c & 0xff;

        this->r = r >> 2 & 0x3f;
        this->g = g >> 2 & 0x3f;
        this->b = b >> 2 & 0x3f;
#elif defined(LCD_COLOR_DEPTH_24)
        this->r = (c >> 16) & 0xff;
        this->g = (c >> 8) & 0xff;
        this->b = c & 0xff;
#endif
    }

    /**
     * @brief Creates a color from 3 values
     * @param r Red value
     * @param g Green value
     * @param b Blue value
    */
    color(uint8_t r, uint8_t g, uint8_t b)
    {
#if defined(LCD_COLOR_DEPTH_1)
        uint32_t temp = (r << 16) | (g << 8) | b;
        this->white = temp >= 0x7fffff;
#else
        this->r = r;
        this->g = g;
        this->b = b;
#endif
    }

    /**
     * @brief Returns the color as a 16 bit value
     * @return 16-bit color
    */
#if defined(LCD_COLOR_DEPTH_1)
    bool toWord()
    {
        return this->white;
    }
#elif defined(LCD_COLOR_DEPTH_8)
    uint8_t toWord()
    {
        return (this->r << 5) | (this->g << 2) | this->b;
    }
#elif defined(LCD_COLOR_DEPTH_16)
    uint16_t toWord()
    {
#if defined(LCD_INVERT_COLORS)
        uint16_t color = (this->r << 11) | (this->g << 5) | this->b;

        color = ((color & 0xaaaa) >> 1) | ((color & 0x5555) << 1);
        color = ((color & 0xcccc) >> 2) | ((color & 0x3333) << 2);
        color = ((color & 0xf0f0) >> 4) | ((color & 0x0f0f) << 4);
        color = (color >> 8) | (color << 8);

        return color;
#else
        return (this->r << 11) | (this->g << 5) | this->b;
#endif
    }
#elif defined(LCD_COLOR_DEPTH_18)
    uint32_t toWord()
    {
#if defined(LCD_INVERT_COLORS)
        uint32_t r6 = r;
        r6 = ((r6 & 0x2a) >> 1) | ((r6 & 0x15) << 1);
        r6 = ((r6 & 0x30) >> 4) | (r6 & 0x0c) | ((r6 & 0x03) << 4);

        uint32_t g6 = g;
        g6 = ((g6 & 0x2a) >> 1) | ((g6 & 0x15) << 1);
        g6 = ((g6 & 0x30) >> 4) | (g6 & 0x0c) | ((g6 & 0x03) << 4);

        uint32_t b6 = b;
        b6 = ((b6 & 0x2a) >> 1) | ((b6 & 0x15) << 1);
        b6 = ((b6 & 0x30) >> 4) | (b6 & 0x0c) | ((b6 & 0x03) << 4);
        return (b6 << 12) | (g6 << 6) | r6;
#else
        return (this->r << 12) | (this->g << 6) | this->b;
#endif
    }
#elif defined(LCD_COLOR_DEPTH_24)
    uint32_t toWord()
    {
#if defined(LCD_INVERT_COLORS)
        uint32_t color = (this->r << 16) | (this->g << 8) | this->b;

        color = ((color & 0xaaaaaa) >> 1) | ((color & 0x555555) << 1);
        color = ((color & 0xcccccc) >> 2) | ((color & 0x333333) << 2);
        color = ((color & 0xf0f0f0) >> 4) | ((color & 0x0f0f0f) << 4);
        color = (color >> 8) | (color << 16);

        return color;
#else
        return (this->r << 16) | (this->g << 8) | this->b;
#endif
    }
#endif

    /**
     * @brief Gets the opposite color
     * @return Opposite color
    */
    color getOppositeColor()
    {
#if defined(LCD_COLOR_DEPTH_1)
        return color(!this->white);
#elif defined(LCD_COLOR_DEPTH_8)
        return color(0x07 - this->r, 0x07 - this->g, 0x03 - this->b);
#elif defined(LCD_COLOR_DEPTH_16)
        return color(0x1f - this->r, 0x3f - this->g, 0x1f - this->b);
#elif defined(LCD_COLOR_DEPTH_18)
        return color(0x3f - this->r, 0x3f - this->g, 0x3f - this->b);
#elif defined(LCD_COLOR_DEPTH_24)
        return color(0xff - this->r, 0xff - this->g, 0xff - this->b);
#endif
    }

    /**
     * @brief Blend two colors
     * @param color color to blend with
     * @param ratio Ratio of the blend
     * @return Blended color
    */
    color blend(color c, uint16_t ratio)
    {
#if defined(LCD_COLOR_DEPTH_1)
        return this.white ? c : *this;
#elif defined(LCD_COLOR_DEPTH_8)
        // split blue and red
        uint8_t rb = c.toWord() & 0x1f;
        rb += ((this->toWord() & 0x1f) - rb) * (ratio >> 2) >> 6;
        // split out green
        uint8_t g = c.toWord() & 0x1c;
        g += ((this->toWord() & 0x1c) - g) * ratio  >> 8;
        // recombine
        uint8_t result = (rb & 0x1f) | (g & 0x1c);

        return color(result);
#elif defined(LCD_COLOR_DEPTH_16)
        // split blue and red
        uint16_t rb = c.toWord() & 0xf81f;
        rb += ((this->toWord() & 0xf81f) - rb) * (ratio >> 2) >> 6;
        // split out green
        uint16_t g = c.toWord() & 0x07e0;
        g += ((this->toWord() & 0x07e0) - g) * ratio  >> 8;
        // recombine
        uint16_t result = (rb & 0xf81f) | (g & 0x07e0);

        return color(result);
#elif defined(LCD_COLOR_DEPTH_18)
        // split blue and red
        uint32_t rb = c.toWord() & 0xfc00fc;
        rb += ((this->toWord() & 0xfc00fc) - rb) * (ratio >> 2) >> 6;
        // split out green
        uint32_t g = c.toWord() & 0x03f000;
        g += ((this->toWord() & 0x03f000) - g) * ratio  >> 8;
        // recombine
        uint32_t result = (rb & 0xfc00fc) | (g & 0x03f000);

        return color(result);
#elif defined(LCD_COLOR_DEPTH_24)
        // split blue and red
        uint32_t rb = c.toWord() & 0xff00ff;
        rb += ((this->toWord() & 0xff00ff) - rb) * (ratio >> 2) >> 6;
        // split out green
        uint32_t g = c.toWord() & 0x00ff00;
        g += ((this->toWord() & 0x00ff00) - g) * ratio  >> 8;
        // recombine
        uint32_t result = (rb & 0xff00ff) | (g & 0x00ff00);

        return color(result);
#endif
    }

    bool operator==(color c)
    {
        return this->r == c.r && this->g == c.g && this->b == c.b;
    }

    bool operator!=(color c)
    {
        return this->r != c.r || this->g != c.g || this->b != c.b;
    }

    color operator+(color c)
    {
        return color(this->r + c.r, this->g + c.g, this->b + c.b);
    }

    color operator-(color c)
    {
        return color(this->r - c.r, this->g - c.g, this->b - c.b);
    }

    color operator*(color c)
    {
        return color(this->r * c.r, this->g * c.g, this->b * c.b);
    }

    color operator/(color c)
    {
        return color(this->r / c.r, this->g / c.g, this->b / c.b);
    }

    color operator<(color c)
    {
        return color(this->r < c.r, this->g < c.g, this->b < c.b);
    }

    color operator>(color c)
    {
        return color(this->r > c.r, this->g > c.g, this->b > c.b);
    }

    color operator<=(color c)
    {
        return color(this->r <= c.r, this->g <= c.g, this->b <= c.b);
    }

    color operator>=(color c)
    {
        return color(this->r >= c.r, this->g >= c.g, this->b >= c.b);
    }

    color biggest(color c)
    {
        return color(this->r > c.r ? this->r : c.r, this->g > c.g ? this->g : c.g, this->b > c.b ? this->b : c.b);
    }

    color smallest(color c)
    {
        return color(this->r < c.r ? this->r : c.r, this->g < c.g ? this->g : c.g, this->b < c.b ? this->b : c.b);
    }
};

#ifdef __cplusplus
}
#endif