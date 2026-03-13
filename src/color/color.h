#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    
#include <stdint.h>

#define MAX_COLOR_DIFF 255

/**
 * @brief Colors enum
 * @note Colors are 16-bit values
*/
typedef enum
{
    black = 0x0000,
    navy = 0x000f,
    darkGreen = 0x03e0,
    darkCyan = 0x03ef,
    maroon = 0x7800,
    purple = 0x780f,
    olive = 0x7be0,
    lightGreen = 0xc618,
    darkGrey = 0x7bef,
    blue = 0x001f,
    green = 0x07e0,
    cyan = 0x07ff,
    red = 0xf800,
    magenta = 0xf81f,
    yellow = 0xffe0,
    white = 0xffff,
    orange = 0xfd20,
    greenYellow = 0xafe5,
    pink = 0xf81f,
    cum = 0xffbd,
    raspberryRed = 0x9861,
    darkOrange = 0xfc60,
    gold = 0xfea0,
    piss = 0xe702,
    derg = 0x0dfb,
    orangeRed = 0xf9e0,
    darkYellow = 0xfe00,
    argent = 0xc618,
    acidGreen = 0xc600,
    islamicGreen = 0x0600,
    turquoiseSurf = 0x0618,
    deepMagenta = 0xc018,
    ueRed = 0xc000,
    mediumBlue = 0x0018,
    chineseBlack = 0x1082,
    oxfordBlue = 0x0109,
    deepViolet = 0x300d,
    vampireBlack = 0x0841,
    eerieBlack = 0x18e3
} colors;

struct color
{
    uint16_t r : 5;
    uint16_t g : 6;
    uint16_t b : 5;
    uint16_t a : 8;

    /**
     * @brief Creates a black color
    */
    color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->a = 0;
    }

    /**
     * @brief Creates a color from a Colors enum
     * @param color Colors enum
    */
    color(colors c)
    {
        this->r = (c >> 11) & 0x1F;
        this->g = (c >> 5) & 0x3F;
        this->b = c & 0x1F;
        this->a = 255;
    }

    /**
     * @brief Creates a color from a 8 bit value
     * @param color 8-bit color
    */
    color(uint8_t c)
    {
        this->r = (c >> 5) & 0x03;
        this->g = (c >> 2) & 0x03;
        this->b = c & 0x02;
        this->a = 255;
    }

    /**
     * @brief Creates a color from a 16 bit value
     * @param color 16-bit color
    */
    color(uint16_t c)
    {
        this->r = (c >> 11) & 0x1F;
        this->g = (c >> 5) & 0x3F;
        this->b = c & 0x1F;
        this->a = 255;
    }

    /**
     * @brief Applies a multiplier to a 24-bit color
     * @param color 24-bit color
     * @param multiplier Multiplier (0-31)
    */
    color(color c, uint16_t multiplier)
    {
        this->r = c.r * multiplier;
        this->g = c.g * multiplier;
        this->b = c.b * multiplier;
        this->a = c.a;
    }

    /**
     * @brief Applies a multiplier to a 16-bit color
     * @param color 16-bit color
     * @param multiplier Multiplier (0-31)
    */
    color(uint16_t c, uint16_t multiplier)
    {
        this->r = ((c >> 11) & 0x1F) * multiplier;
        this->g = ((c >> 5) & 0x3F) * multiplier;
        this->b = (c & 0x1F) * multiplier;
        this->a = 255;
    }

    /**
     * @brief Creates a color from 3 values
     * @param r Red value (5 bits)
     * @param g Green value (6 bits)
     * @param b Blue value (5 bits)
    */
    color(uint16_t r, uint16_t g, 
        uint16_t b, uint16_t a = 255)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    /**
     * @brief Returns the color as a 16 bit value
     * @return 16-bit color
    */
    uint16_t to16bit()
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

    /**
     * @brief Converts a 24-bit color to a 16-bit color
     * @param color 24-bit color
     * @return 16-bit color
     */
    color hexToColor(uint32_t c)
    {
        uint8_t r = (c >> 16) & 0xFF;
        uint8_t g = (c >> 8) & 0xFF;
        uint8_t b = c & 0xFF;

        this->r = r >> 3 & 0x1F;
        this->g = g >> 2 & 0x3F;
        this->b = b >> 3 & 0x1F;
        this->a = 255;

        return *this;
    }

    /**
     * @brief Gets the opposite color
     * @return Opposite color
    */
    color getOppositeColor()
    {
        return color(0x1f - this->r, 0x3f - this->g, 0x1f - this->b);
    }

    /**
     * @brief Blend two colors
     * @param color color to blend with
     * @param ratio Ratio of the blend (0-1)
     * @return Blended color
    */
    color blend(color c, uint16_t ratio)
    {
        // split blue and red
        uint16_t rb = c.to16bit() & 0xf81f;
        rb += ((this->to16bit() & 0xf81f) - rb) * (ratio >> 2) >> 6;
        // split out green
        uint16_t g = c.to16bit() & 0x07e0;
        g += ((this->to16bit() & 0x07e0) - g) * ratio  >> 8;
        // recombine
        uint16_t result = (rb & 0xf81f) | (g & 0x07e0);

        return color(result);
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