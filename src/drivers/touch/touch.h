#pragma once
#include "lcd_config.h"
#if !defined(TOUCH_DISABLED)

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

#if defined(TOUCH_I2C_INSTANCE_I2C0)
    i2c_inst_t *i2c_ i2c_ptr = i2c0;
#elif defined(TOUCH_I2C_INSTANCE_I2C1)
    i2c_inst_t *i2c_ptr = i2c1;
#else
#error "Invalid I2C instance"
#endif

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
#endif