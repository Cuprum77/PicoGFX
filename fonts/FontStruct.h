
#pragma once

// Estimated total memory usage: 532380 bytes

// Declare a function pointer type
typedef const unsigned int* (*FontBitmapFunctionType)(const char);

// Struct for storing the font data
typedef struct {
    FontBitmapFunctionType function;
    unsigned int size;
    unsigned int width;
    unsigned int height;
} FontStruct;
