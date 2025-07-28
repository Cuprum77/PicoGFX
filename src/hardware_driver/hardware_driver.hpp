#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_spi.pio.h"
#include "display_struct.h"

typedef enum
{
    BITS_8 = 8,
    BITS_16 = 16
}  spi_bit_length_t;

typedef struct pio_spi_inst 
{
    PIO pio;
    uint32_t sm;
    uint32_t cs_pin;
} pio_spi_inst_t;

class hardware_driver
{
public:
    hardware_driver(display_config_t* config);
    void init(void);
    void reset(uint32_t time_ms);

    void writeData(uint8_t command, const uint8_t* data, size_t length);
    void setDataMode(uint8_t command);
    void writePixels(const uint16_t* data, size_t length);

private:
    // general stuff
    display_config_t* config;
    display_interface_t interface;
    bool pioMode = false;   // hw mode only

    // dma stuff
    uint32_t dma_tx;
    dma_channel_config dma_config;

    // pio stuff
    PIO pio;
    uint32_t sm;
    uint32_t offset;
    float clkdiv;

    // general
    bool enabled = false;
    bool parallel_interface_in_sequence = false;
    bool parallel_interface_reverse_order = false;
    uint32_t parallel_interface_mask = 0;
    uint32_t parallel_interface_min_pin = 0;
    uint32_t parallel_interface_max_pin = 0;

    // private functions
    void initSPI(void);
    void initSPIwPIO(void);
    void init8080(void);
    void changeSPIbits(spi_bit_length_t bits);
    void write8080(uint32_t data, bool command, bool bit16);
    inline void setSPIdataCommandPins(bool dc, bool cs);
    bool isSequential(uint32_t* arr, size_t size);
    uint16_t flipBits(uint16_t value);
};