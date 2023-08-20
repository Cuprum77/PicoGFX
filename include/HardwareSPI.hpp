#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_spi.pio.h"
#include "Commands.hpp"
#include "Structs.h"

typedef enum
{
    SPI_HW,
    SPI_DMA_HW,
    PIO_HW,
    PIO_DMA_HW
}   SPI_Interface_t;

typedef enum
{
    BITS_8 = 8,
    BITS_16 = 16,
    BITS_18 = 18
}  SPI_Bits_t;

typedef struct pio_spi_inst 
{
    PIO pio;
    uint sm;
    uint cs_pin;
} pio_spi_inst_t;

struct Hardware_Params
{
    SPI_Interface_t hw_interface;
    spi_inst_t* spi_instance;
    unsigned int baudrate;
};

class HardwareSPI
{
public:
    HardwareSPI(Display_Pins pins, Hardware_Params hw_params);
    void init(void);

    void spi_write_data(uint8_t command, const uint8_t* data, size_t length);
    void spi_set_data_mode(uint8_t command);
    void spi_write_pixels(const uint16_t* data, size_t length);

    bool dma_busy(void);

private:
    // general stuff
    Hardware_Params hw_params;

    // spi stuff
    spi_inst_t* spi;
    uint8_t scl;
    uint8_t sda;
    uint8_t dc;
    uint8_t cs;

    // dma stuff
    uint dma_tx;
    dma_channel_config dma_config;

    // pio stuff
    PIO pio;
    uint sm;
    uint offset;
    float clkdiv;

    // general
    bool enabled = false;

    // private functions
    void initSPI(void);
    void initSPIwDMA(void);
    void initPIO(void);
    void initPIOwDMA(void);
    void changePIOSettings(SPI_Bits_t bits);
    inline void set_dc_cs(bool dc, bool cs);
};