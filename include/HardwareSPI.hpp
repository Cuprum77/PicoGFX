#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "Commands.hpp"

class HardwareSPI
{
public:
    HardwareSPI(spi_inst_t* spi, uint baudrate, uint8_t dc, uint8_t cs, uint8_t scl, uint8_t sda, bool dma = false);

    int spi_write_data(uint8_t command, const uint8_t* data, size_t length);
    int spi_set_data_mode(uint8_t command);
    int spi_write_pixels(const uint16_t* data, size_t length);

    bool dma_busy();

private:
    // spi stuff
    spi_inst_t* spi;
    uint baudrate;
    uint8_t scl;
    uint8_t sda;
    uint8_t dc;
    uint8_t cs;

    // dma stuff
    bool dma;
    uint dma_tx;
    dma_channel_config dma_config;

    void initSPI();
    void initDMA();
};