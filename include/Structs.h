#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

// Only include on a pico
#ifdef PICO_BUILD
#include "hardware/spi.h"
#endif

typedef struct
{
    int rst;
    int dc;
    int cs;
    int sda;
    int scl;

    bool pio;
#ifdef PICO_BUILD
    spi_inst_t* spi_instance;
#endif
    unsigned int baudrate;
} display_spi_config_t;

typedef struct
{
    int db0;
    int db1;
    int db2;
    int db3;
    int hsync;
    int vsync;
    int vclk;
} display_piss_config_t;

typedef struct
{
    int db0;
    int db1;
    int db2;
    int db3;
    int db4;
    int db5;
    int db6;
    int db7;
    int db8;
    int db9;
    int db10;
    int db11;
    int db12;
    int db13;
    int db14;
    int db15;
    int db16;
    int db17;
    int hs;
    int vs;
    int dclk;
} display_rgb_config_t;

typedef enum
{
    DISPLAY_SPI,
    DISPLAY_PISS,
    DISPLAY_RGB
} display_interface_t;


typedef struct
{
    bool dimming;
    int backlightPin;
    unsigned int height;
    unsigned int width;
    unsigned int columnOffset1;
    unsigned int columnOffset2;
    unsigned int rowOffset1;
    unsigned int rowOffset2;   
    unsigned int rotation;

#ifdef PICO_BUILD
    display_interface_t interface;
    display_spi_config_t spi;
    display_piss_config_t piss;
    display_rgb_config_t rgb;
#endif
} display_config_t;

#ifdef __cplusplus
}
#endif