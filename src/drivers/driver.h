#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_spi.pio.h"
#include "pio_qspi.pio.h"
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

    // SPI PIO instance
#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(void *data, size_t length);
    inline void pio_set_bits(uint32_t bits);
    inline void set_spi_dc_cs(bool dc, bool cs);

    // SPI instance
#elif defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
#if defined(LCD_SPI_INSTANCE_SPI0)
    spi_inst_t *spi_instance = spi0;
#elif defined(LCD_SPI_INSTANCE_SPI1)
    spi_inst_t *spi_instance = spi1;
#else
#error "Invalid SPI instance"
#endif
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(void *data, size_t length);

    // QSPI instance
#elif defined(LCD_PROTOCOL_QSPI)
#if defined(LCD_PIN_DAT_SEQUENTIAL)
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(void *data, size_t length);
    inline void pio_set_bits(uint32_t bits);
    inline void set_spi_dc_cs(bool dc, bool cs);
#else
#error "QSPI protocol requires sequential data pins"
#endif

    // I2C instance
#elif defined(LCD_PROTOCOL_I2C) 
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(void *data, size_t length);

    // Parallel interface
#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    uint32_t parallel_data_pins[LCD_PIN_DB_COUNT] = LCD_PIN_DB_ARRAY;

    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void write8080(uint32_t data, bool command, bool bit16);
    inline void protocol_write_pixels(void *data, size_t length);

#else
#error "Unsupported display protocol or hardware interface"

#endif
};