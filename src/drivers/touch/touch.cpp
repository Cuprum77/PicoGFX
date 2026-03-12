#include "touch.hpp"

touch::touch(display_touch_config_t* config)
{
    this->i2c_inst = config->i2c_inst;
    this->sda_pin = config->sda_pin;
    this->scl_pin = config->scl_pin;
    this->speed = config->speed;
}

/**
 * @protected
 * @brief Initialize the touch controller
 */
void touch::initI2C()
{
    i2c_init(this->i2c_inst, this->speed);

    gpio_set_function(this->sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(this->scl_pin, GPIO_FUNC_I2C);

    gpio_pull_up(this->sda_pin);
    gpio_pull_up(this->scl_pin);
}

/**
 * @public
 * @brief Check if data is ready
 * @returns True if data is ready
 * @note This function will reset the ready flag on read!
 */
bool touch::ready()
{
    bool available = this->available;
    this->available = false;
    return available;
}

/**
 * @protected
 * @brief Read a single byte from the touch controller
 * @param addr I2C address
 * @param reg Register to read from
 * @param data Byte to read
 * @return length of the data read
 */
uint32_t touch::touchRead(uint8_t addr, uint8_t reg, uint8_t* data)
{
    return this->touchRead(addr, reg, data, 1);
}

/**
 * @protected
 * @brief Read data from the touch controller
 * @param addr I2C address
 * @param reg Register to read from
 * @param data Data to read
 * @param len Length of the data
 * @return length of the data read
 */
uint32_t touch::touchRead(uint8_t addr, uint8_t reg, uint8_t* data, size_t len)
{
    uint32_t bytes_written = 0;

    bytes_written = i2c_write_blocking(this->i2c_inst, addr, &reg, 1, true);
    bytes_written += i2c_read_blocking(this->i2c_inst, addr, data, len, false);

    return bytes_written;
}

/**
 * @protected
 * @brief Read a single byte from the touch controller
 * @param addr I2C address
 * @param reg Register to read from
 * @param data Byte to read
 * @return length of the data read
 */
uint32_t touch::touchRead(uint8_t addr, uint16_t reg, uint8_t* data, bool msb_first)
{
    return this->touchRead(addr, reg, data, 1, msb_first);
}

/**
 * @protected
 * @brief Read data from the touch controller
 * @param addr I2C address
 * @param reg Register to read from
 * @param data Data to read
 * @param len Length of the data
 * @return length of the data read
 */
uint32_t touch::touchRead(uint8_t addr, uint16_t reg, uint8_t* data, size_t len, bool msb_first)
{
    uint32_t bytes_written = 0;
    uint8_t buf[2] = { 0 };

    if (msb_first)
    {
        buf[0] = (uint8_t)(reg >> 8);
        buf[1] = (uint8_t)(reg & 0xff);
    }
    else
    {
        buf[0] = (uint8_t)(reg & 0xff);
        buf[1] = (uint8_t)(reg >> 8);
    }

    bytes_written = i2c_write_blocking(this->i2c_inst, addr, buf, 2, true);
    bytes_written += i2c_read_blocking(this->i2c_inst, addr, data, len, false);

    return bytes_written;
}

/**
 * @protected
 * @brief Write a single byte to the touch controller
 * @param addr I2C address
 * @param reg Register to write to
 * @param data Byte to write
 * @return length of the data written
 */
uint32_t touch::touchWrite(uint8_t addr, uint8_t reg, uint8_t data)
{
    return this->touchWrite(addr, reg, &data, 1);
}

/**
 * @protected
 * @brief Write data to the touch controller
 * @param addr I2C address
 * @param reg Register to write to
 * @param data Data to write
 * @param len Length of the data
 * @return length of the data written
 */
uint32_t touch::touchWrite(uint8_t addr, uint8_t reg, uint8_t* data, size_t len)
{
    uint32_t bytes_written = 0;

    bytes_written = i2c_write_blocking(this->i2c_inst, addr, &reg, 1, true);
    bytes_written += i2c_write_blocking(this->i2c_inst, addr, data, len, false);

    return bytes_written;
}

/**
 * @protected
 * @brief Write a single byte to the touch controller
 * @param addr I2C address
 * @param reg Register to write to
 * @param data Byte to write
 * @return length of the data written
 */
uint32_t touch::touchWrite(uint8_t addr, uint16_t reg, uint8_t data, bool msb_first)
{
    return this->touchWrite(addr, reg, &data, 1, msb_first);
}

/**
 * @protected
 * @brief Write data to the touch controller
 * @param addr I2C address
 * @param reg Register to write to
 * @param data Data to write
 * @param len Length of the data
 * @return length of the data written
 */
uint32_t touch::touchWrite(uint8_t addr, uint16_t reg, uint8_t* data, size_t len, bool msb_first)
{
    uint32_t bytes_written = 0;
    uint8_t buf[2] = { 0 };

    if (msb_first)
    {
        buf[0] = (uint8_t)(reg >> 8);
        buf[1] = (uint8_t)(reg & 0xff);
    }
    else
    {
        buf[0] = (uint8_t)(reg & 0xff);
        buf[1] = (uint8_t)(reg >> 8);
    }
    
    bytes_written = i2c_write_blocking(this->i2c_inst, addr, buf, 2, true);
    bytes_written += i2c_write_blocking(this->i2c_inst, addr, data, len, false);

    return bytes_written;
}