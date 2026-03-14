#include "cst816.h"
#if defined(TOUCH_DRIVER_CST816)

cst816 *cst816::instance = nullptr;  // Define the static instance

/**
 * @public
 * @brief CST816 constructor
 * @param config Touch configuration pointer
 * @param display Display pointer
 * @note This constructor will setup a IRQ handler for the CST816 as the CST816 will not respond unless touch is detected!
 */
cst816::cst816(display_obj *display_ptr) : touch()
{
    this->display_ptr = display_ptr;
    this->instance = this;
}

/**
 * @public
 * @brief Init the CST816
 */
void cst816::init()
{
#if defined(TOUCH_ENABLED)
    this->initI2C();

    gpio_init(TOUCH_I2C_IRQ_PIN);
    gpio_set_dir(TOUCH_I2C_IRQ_PIN, GPIO_IN);
    gpio_pull_up(TOUCH_I2C_IRQ_PIN);

    gpio_init(TOUCH_I2C_RST_PIN);
    gpio_set_dir(TOUCH_I2C_RST_PIN, GPIO_OUT);
    this->reset();

    gpio_set_irq_enabled_with_callback(TOUCH_I2C_IRQ_PIN, GPIO_IRQ_EDGE_FALL, true, &cst816::static_irq_handler);
#endif
}

/**
 * @public
 * @brief Reset the CST816
 */
void cst816::reset()
{
#if defined(TOUCH_ENABLED)
    gpio_put(TOUCH_I2C_RST_PIN, 1);
    sleep_ms(50);
    gpio_put(TOUCH_I2C_RST_PIN, 0);
    sleep_ms(5);
    gpio_put(TOUCH_I2C_RST_PIN, 1);
#endif
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

    if (this->display_ptr != nullptr)
    {    
        // Translate the X and Y to the display
        uint32_t rotation = this->display_ptr->getRotation();

        switch (rotation)
        {
            case 0:
                break;
            case 90:
                std::swap(x, y);
                y = this->display_ptr->getHeight() - y;
                break;
            case 180:
                x = this->display_ptr->getWidth() - x;
                y = this->display_ptr->getHeight() - y;
                break;
            case 270:
                std::swap(x, y);
                x = this->display_ptr->getHeight() - x;
                break;
            default:
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
void cst816::getVersionValue(uint32_t *data)
{
    uint8_t buf[3] = { 0 };
    this->touchRead(CST816_ADDR, (uint8_t)0xa7, buf, 3);
    *data = (buf[0] << 16) | (buf[1] << 8) | buf[2];
}
#endif