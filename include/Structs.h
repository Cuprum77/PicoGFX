#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "hardware/spi.h"


typedef struct
{
    int rst;
    int dc;
    int cs;
    int sda;
    int scl;

    bool pio;
    spi_inst_t* spi_instance;
    unsigned int baudrate;
} display_spi_config_t;

typedef enum
{
    DISPLAY_SPI,
} display_interface_t;


typedef struct
{
    display_interface_t interface;
    bool dimming;
    int backlightPin;
    unsigned int height;
    unsigned int width;
    unsigned int columnOffset1;
    unsigned int columnOffset2;
    unsigned int rowOffset1;
    unsigned int rowOffset2;   
    unsigned int rotation;

    display_spi_config_t spi;
} display_config_t;

#ifdef __cplusplus
}
#endif