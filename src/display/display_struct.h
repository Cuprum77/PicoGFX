#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

// Only include on a pico
#ifdef PICO_BUILD
#include "hardware/spi.h"
#include "hardware/i2c.h"
#endif

typedef struct
{
    int32_t rst;
    int32_t dc;
    int32_t cs;
    int32_t sda;
    int32_t scl;

    bool pio;
#ifdef PICO_BUILD
    spi_inst_t* spi_instance;
#endif
    uint32_t baudrate;
} display_spi_config_t;

typedef struct
{
    int32_t db0;
    int32_t db1;
    int32_t db2;
    int32_t db3;
    int32_t hsync;
    int32_t vsync;
    int32_t vclk;
} display_piss_config_t;

#ifdef PICO_BUILD
typedef struct
{
    i2c_inst_t* i2c_inst;
    uint32_t speed;
    uint32_t sda_pin;
    uint32_t scl_pin;
    uint32_t irq_pin;
    uint32_t rst_pin;
} display_touch_config_t;
#endif

typedef struct
{
    int32_t db0;
    int32_t db1;
    int32_t db2;
    int32_t db3;
    int32_t db4;
    int32_t db5;
    int32_t db6;
    int32_t db7;
    int32_t db8;
    int32_t db9;
    int32_t db10;
    int32_t db11;
    int32_t db12;
    int32_t db13;
    int32_t db14;
    int32_t db15;
    int32_t db16;
    int32_t db17;
    int32_t hs;
    int32_t vs;
    int32_t dclk;
} display_rgb_config_t;

typedef enum
{
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270
} display_rotation_t;

typedef enum
{
    DISPLAY_SPI,
    DISPLAY_PISS,
    DISPLAY_RGB
} display_interface_t;


typedef struct
{
    bool dimming;
    int32_t backlightPin;
    uint32_t height;
    uint32_t width;
    uint32_t columnOffset1;
    uint32_t columnOffset2;
    uint32_t rowOffset1;
    uint32_t rowOffset2;   
    display_rotation_t rotation;

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