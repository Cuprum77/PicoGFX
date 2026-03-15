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
    uint32_t db[24];
    size_t db_size;
    uint32_t rst;
    uint32_t csx;
    uint32_t dcx;
    uint32_t rdx;
    uint32_t wrx;
    uint32_t im0;
    bool pio;
} display_8080_config_t;

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
    DISPLAY_8080,
} display_interface_t;


typedef struct
{
    bool dimming;
    bool inverseColors;
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
    display_8080_config_t b8080;
#endif
} display_config_t;

#ifdef __cplusplus
}
#endif