#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <hardware/i2c.h>
#include "pico/stdlib.h"

class touch
{
public:
    bool ready();

protected:
    bool available = false;

    void initI2C();

    uint32_t touchRead(uint8_t addr, uint8_t reg, uint8_t *data);
    uint32_t touchRead(uint8_t addr, uint16_t reg, uint8_t *data, bool msb_first = true);
    uint32_t touchRead(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);
    uint32_t touchRead(uint8_t addr, uint16_t reg, uint8_t *data, size_t len, bool msb_first = true);

    uint32_t touchWrite(uint8_t addr, uint8_t reg, uint8_t data);
    uint32_t touchWrite(uint8_t addr, uint16_t reg, uint8_t data, bool msb_first = true);
    uint32_t touchWrite(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);
    uint32_t touchWrite(uint8_t addr, uint16_t reg, uint8_t *data, size_t len, bool msb_first = true);
};