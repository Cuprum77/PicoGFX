#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

enum display_type_t
{
    ST7789,
    GC9A01
};

struct Display_Pins
{
    int rst;
    int dc;
    int cs;
    int sda;
    int scl;
    int bl;
};

struct Display_Params
{
    display_type_t type;
    bool dimming;
    unsigned int height;
    unsigned int width;
    unsigned int columnOffset1;
    unsigned int columnOffset2;
    unsigned int rowOffset1;
    unsigned int rowOffset2;   
    unsigned int rotation;
};

#ifdef __cplusplus
}
#endif