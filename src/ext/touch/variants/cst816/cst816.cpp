#include "cst816.hpp"

cst816* cst816::instance = nullptr;  // Define the static instance

/**
 * @public
 * @brief CST816 constructor
 * @param config Touch configuration pointer
 * @param display Display pointer
 * @note This constructor will setup a IRQ handler for the CST816 as the CST816 will not respond unless touch is detected!
 */
cst816::cst816(display_touch_config_t* config, Display* display) : touch(config)
{
    this->irq_pin = config->irq_pin;
    this->rst_pin = config->rst_pin;
    this->display = display;
    this->instance = this;
}

/**
 * @public
 * @brief Init the CST816
 */
void cst816::init()
{
    this->initI2C();

    gpio_init(this->irq_pin);
    gpio_set_dir(this->irq_pin, GPIO_IN);
    gpio_pull_up(this->irq_pin);

    gpio_init(this->rst_pin);
    gpio_set_dir(this->rst_pin, GPIO_OUT);
    this->reset();

    gpio_set_irq_enabled_with_callback(this->irq_pin, GPIO_IRQ_EDGE_FALL, true, &cst816::static_irq_handler);
}

/**
 * @public
 * @brief Reset the CST816
 */
void cst816::reset()
{
    gpio_put(this->rst_pin, 1);
    sleep_ms(50);
    gpio_put(this->rst_pin, 0);
    sleep_ms(5);
    gpio_put(this->rst_pin, 1);
}

/**
 * @public
 * @brief Disable auto sleep on the CST816
 */
void cst816::disableStandby()
{
    this->touchWrite(CST816_ADDR, (uint8_t)0xfe, 0xfe);
}

/**
 * @public
 * @brief Enable auto sleep on the CST816
 */
void cst816::enableStandby()
{
    this->touchWrite(CST816_ADDR, (uint8_t)0xfe, 0x00);
}

/**
 * @public
 * @brief Get the X position of the touch
 * @returns X position
 */
uint32_t cst816::getX()
{
    return this->report.x;
}

/**
 * @public
 * @brief Get the Y position of the touch
 * @returns Y position
 */
uint32_t cst816::getY()
{
    return this->report.y;
}

/**
 * @private
 * @brief Fetch the touch data
 */
void cst816::fetch()
{
    uint8_t data[6] = { 0 };
    this->touchRead(CST816_ADDR, (uint8_t)0x01, data, 6);

    uint16_t x = (uint16_t)(((data[2] & 0x0f) << 8) | data[3]);
    uint16_t y = (uint16_t)(((data[4] & 0x0f) << 8) | data[5]);

    if (this->display != nullptr)
    {    
        // Translate the X and Y to the display
        display_rotation_t rotation = this->display->getRotation();

        switch (rotation)
        {
            case display_rotation_t::ROTATION_0:
                break;
            case display_rotation_t::ROTATION_90:
                std::swap(x, y);
                y = this->display->getHeight() - y;
                break;
            case display_rotation_t::ROTATION_180:
                x = this->display->getWidth() - x;
                y = this->display->getHeight() - y;
                break;
            case display_rotation_t::ROTATION_270:
                std::swap(x, y);
                x = this->display->getHeight() - x;
                break;
        }
    }

    this->report = {
        .gesture = data[0],
        .points = data[1],
        .x = x,
        .y = y
    };
    
    this->available = true;
}

/**
 * @private
 * @brief Get the version of the CST816
 * @returns Version
 */
uint32_t cst816::getVersion()
{
    uint8_t data = 0;
    this->touchRead(CST816_ADDR, (uint8_t)0x15, &data);
    return data;
}

/**
 * @private
 * @brief Get the version of the CST816
 * @param data Version data
 */
void cst816::getVersionValue(uint32_t* data)
{
    uint8_t buf[3] = { 0 };
    this->touchRead(CST816_ADDR, (uint8_t)0xa7, buf, 3);
    *data = (buf[0] << 16) | (buf[1] << 8) | buf[2];
}