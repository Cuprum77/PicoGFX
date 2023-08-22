#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_spi.pio.h"
#include "Structs.h"

typedef enum
{
    BITS_8 = 8,
    BITS_16 = 16
}  spi_bit_length_t;

typedef struct pio_spi_inst 
{
    PIO pio;
    uint sm;
    uint cs_pin;
} pio_spi_inst_t;

class Driver
{
public:
    Driver(display_config_t* config);
    void init(void);

    void writeData(uint8_t command, const uint8_t* data, size_t length);
    void setDataMode(uint8_t command);
    void writePixels(const uint16_t* data, size_t length);

private:
    // general stuff
    display_config_t* config;
    display_interface_t interface;
    bool pioMode = false;   // SPI mode only

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
    void initSPIwPIO(void);
    void initPiss(void);
    void initRGB(void);
    void changeSPIbits(spi_bit_length_t bits);
    inline void setSPIdataCommandPins(bool dc, bool cs);
};