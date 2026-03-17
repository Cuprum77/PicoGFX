#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "lcd_config.h"
#include "color.h"

#if defined(LCD_HARDWARE_PIO)
#include "pio_spi.pio.h"

#elif defined(LCD_PROTOCOL_QSPI) && defined(LCD_HARDWARE_PIO)
#include "pio_qspi.pio.h"

#elif defined(LCD_HARDWARE_RGB)
#if defined(LCD_PROTOCOL_PARALLEL_8)
#include "pio_rgb8.pio.h"
#elif defined(LCD_PROTOCOL_PARALLEL_16)
#include "pio_rgb16.pio.h"
#elif defined(LCD_PROTOCOL_PARALLEL_24)
#include "pio_rgb24.pio.h"
#endif
#endif

#if defined(LCD_PROTOCOL_PARALLEL_8) || defined(LCD_PROTOCOL_PARALLEL_16) || defined(LCD_PROTOCOL_PARALLEL_24)
#if defined(LCD_COLOR_DEPTH_18)
#error "Parallel interface does not support 18 bit color depth"
#endif

#if defined(LCD_COLOR_DEPTH_8) && !defined(LCD_PROTOCOL_PARALLEL_8)
#if defined(LCD_PROTOCOL_PARALLEL_16)
// Match the color depth to the available pins if the user has not set it to 8 bit
#define PCONV(x) (((((x & 0xf800) >> 11) & 0x7) << 5) | \
    ((((x & 0x07e0) >> 5) & 0x7) << 2) | \
    ((((x & 0x001f) >> 0) & 0x3) << 0))

#elif defined(LCD_PROTOCOL_PARALLEL_24)
#define PCONV(x) (((((x & 0xff0000) >> 16) & 0x7) << 5) | \
    ((((x & 0x00ff00) >> 8) & 0x7) << 2) | \
    ((((x & 0x0000ff) >> 0) & 0x3) << 0))

#else
#define PCONV(x) x

#endif
#elif defined(LCD_COLOR_DEPTH_16) && !defined(LCD_PROTOCOL_PARALLEL_16)
#if defined(LCD_PROTOCOL_PARALLEL_8)
#define PCONV(x) (((((x & 0xe0) >> 5) & 0x1f) << 11) | \
    ((((x & 0x1c) >> 2) & 0x3f) << 5) | \
    (((x & 0x03) >> 0) & 0x1f))

#elif defined(LCD_PROTOCOL_PARALLEL_24)
#define PCONV(x) (((((x & 0xff0000) >> 16) & 0x1f) << 11) | \
    ((((x & 0x00ff00) >> 8) & 0x3f) << 5) | \
    (((x & 0x0000ff) >> 0) & 0x1f))

#else
#define PCONV(x) x

#endif

#elif defined(LCD_COLOR_DEPTH_24) && !defined(LCD_PROTOCOL_PARALLEL_24)
#if defined(LCD_PROTOCOL_PARALLEL_8)
#define PCONV(x) (((((x & 0xe0) >> 5) & 0xff) << 16) | \
    ((((x & 0x1c) >> 2) & 0xff) << 8) | \
    (((x & 0x03) >> 0) & 0xff))

#elif defined(LCD_PROTOCOL_PARALLEL_16)
#define PCONV(x) (((((x & 0xf800) >> 11) & 0xff) << 16) | \
    ((((x & 0x07e0) >> 5) & 0xff) << 8) | \
    (((x & 0x001f) >> 0) & 0xff))

#else
#define PCONV(x) x

#endif

#else
#define PCONV(x) x

#endif
#endif

#if defined(SPI) && (!defined(GPIO) || !defined(PIO))
#error "SPI protocol requires GPIO or PIO hardware interface"
#endif

#if defined(QSPI) && (!defined(GPIO) || !defined(PIO))
// #error "QSPI protocol requires GPIO or PIO hardware interface"
#endif

#if defined(QSPI) && !defined(LCD_PIN_DAT_SEQUENTIAL)
#error "QSPI protocol requires sequential data pins"
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
    void writePixels(const color_t *data, size_t length);
    void setDataMode(uint8_t command);
    void switchTransmissionMode(bool data);

private:
    // general
    bool enabled = false;
    bool speed_mode = false;
    uint32_t parallel_interface_mask = 0;
    uint32_t parallel_interface_min_pin = 0;
    uint32_t parallel_interface_max_pin = 0;

#if defined(LCD_HARDWARE_PIO)
    PIO pio;
    uint32_t sm;
    uint32_t offset;
    float clkdiv;
#elif defined(LCD_HARDWARE_RGB)
    PIO pio;

    uint32_t sm_hsync;
    uint32_t sm_vsync;
    uint32_t sm_rgb;
    uint32_t sm_de;

    uint32_t off_hsync;
    uint32_t off_vsync;
    uint32_t off_rgb;
    uint32_t off_de;

    uint32_t pixel_clock_hz = LCD_DCLK_FREQ;
#endif

    // SPI PIO instance
#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(const color_t *data, size_t length);
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
    inline void protocol_write_pixels(const color_t *data, size_t length);

    // QSPI instance
#elif defined(LCD_PROTOCOL_QSPI)
#if defined(LCD_PIN_DAT_SEQUENTIAL)
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length, bool keep_cs = false);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(const color_t *data, size_t length);
    inline void pio_set_bits(uint32_t bits);
    inline void send_command_over_spi(uint8_t command);
#else
#error "QSPI protocol requires sequential data pins"
#endif

    // I2C instance
#elif defined(LCD_PROTOCOL_I2C) 
    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
    inline void protocol_write_pixels(const color_t *data, size_t length);

    // Parallel interface
#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    uint32_t parallel_data_pins[LCD_PIN_DB_COUNT] = LCD_PIN_DB_ARRAY;

    inline void protocol_init();
    inline void protocol_write_data(uint8_t command, const uint8_t *data, size_t length);
    inline void protocol_set_data_mode(uint8_t command);
#if defined(LCD_HARDWARE_GPIO)    
    inline void write8080data(uint8_t data, bool command);
#endif
    inline void write_parallel_pixels(const color_t data);
    inline void protocol_write_pixels(const color_t *data, size_t length);
#else
#error "Unsupported display protocol or hardware interface"

#endif
};