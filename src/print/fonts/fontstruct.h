#pragma once

#include <stdint.h>

// Estimated total memory usage: 107804 bytes

// Struct for storing the location of the bitmap in memory
// Offset is the number of pixels from the upper left corner
typedef struct {
    uint32_t pointer;
    uint32_t length;
    uint8_t width;
    uint8_t height;
    int8_t xOffset;
    int8_t yOffset;
} FontCharacter;


// Struct for storing the font data
typedef struct {
    const uint32_t *bitmap;
    const FontCharacter *characters;
    uint32_t size;
    uint32_t newLineDistance;
} FontStruct;
