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

#if defined(LCD_PROTOCOL_PARALLEL_24) && !defined(LCD_COLOR_DEPTH_24)
#error "Parallel 24-bit protocol requires 24-bit color depth"
#endif

#if defined(LCD_PROTOCOL_PARALLEL_16) && !defined(LCD_COLOR_DEPTH_16)
#error "Parallel 16-bit protocol requires 16-bit color depth"
#endif

#if defined(LCD_PROTOCOL_PARALLEL_8) && !defined(LCD_COLOR_DEPTH_8)
#error "Parallel 8-bit protocol requires 8-bit color depth"
#endif

#if defined(SPI) && (!defined(GPIO) || !defined(PIO))
#error "SPI protocol requires GPIO or PIO hardware interface"
#endif

typedef enum
{
    BITS_8 = 8,
    BITS_16 = 16,
    BITS_18 = 18,
    BITS_24 = 24
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
    
#if defined(LCD_COLOR_DEPTH_1)
    void writePixels(const bool *data, size_t length);
#elif defined(LCD_COLOR_DEPTH_8)
    void writePixels(const uint8_t *data, size_t length);
#elif defined(LCD_COLOR_DEPTH_16)
    void writePixels(const uint16_t *data, size_t length);
#elif defined(LCD_COLOR_DEPTH_18) || defined(LCD_COLOR_DEPTH_24)
    void writePixels(const uint32_t *data, size_t length);
#else
#error "Unsupported color depth"
#endif

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

    // spi stuff
#if defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
#if defined(LCD_SPI_INSTANCE_SPI0)
    spi_inst_t *spi_instance = spi0;
#elif defined(LCD_SPI_INSTANCE_SPI1)
    spi_inst_t *spi_instance = spi1;
#else
#error "Invalid SPI instance"
#endif
#endif

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
    void set_spi_format(void);
};