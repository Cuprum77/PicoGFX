#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_spi.pio.h"
#include "lcd_config.h"

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
    void init(void);
    void reset(uint32_t time_ms);

    void writeData(uint8_t command, const uint8_t *data, size_t length);
    void setDataMode(uint8_t command);
    void writePixels(const uint16_t *data, size_t length);

private:
    // dma stuff
    uint32_t dma_tx;
    dma_channel_config dma_config;

#if defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    uint32_t parallel_data_pins[LCD_PIN_DB_COUNT] = LCD_PIN_DB_ARRAY;
#endif

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
    void changeSPIbits(spi_bit_length_t bits);
    void write8080(uint32_t data, bool command, bool bit16);
    inline void setSPIdataCommandPins(bool dc, bool cs);
    uint16_t flipBits(uint16_t value);
};