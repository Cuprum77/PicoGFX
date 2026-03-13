#include "gt911.h"

gt911* gt911::instance = nullptr;  // Define the static instance

/**
 * @public
 * @brief GT911 constructor
 * @param config Touch configuration pointer
 * @param display Display pointer
 */
gt911::gt911(display* display_ptr) : touch()
{
    this->display_ptr = display_ptr;
    this->instance = this;
}

/**
 * @public
 * @brief Init the GT911
 */
void gt911::init()
{
#if defined(TOUCH_ENABLED)
    this->initI2C();

    gpio_init(TOUCH_I2C_IRQ_PIN);
    gpio_set_dir(TOUCH_I2C_IRQ_PIN, GPIO_IN);
    gpio_pull_up(TOUCH_I2C_IRQ_PIN);

    gpio_init(TOUCH_I2C_RST_PIN);
    gpio_set_dir(TOUCH_I2C_RST_PIN, GPIO_OUT);
    this->reset();

    gpio_set_irq_enabled_with_callback(TOUCH_I2C_IRQ_PIN, GPIO_IRQ_EDGE_FALL, true, &gt911::static_irq_handler);

    // Set the number of touches
    uint8_t data = 0x05;
    this->touchWrite(GT911_ADDR, (uint16_t)GT911_REG_TOUCH_NUM, data);

    // Get the config data
    uint8_t buffer[11] = { 0 };
    this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_PID, buffer, 4, true);
    this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_FID, buffer + 4, 2, true);
    this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_X_RES, buffer + 6, 2, true);
    this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_Y_RES, buffer + 8, 2, true);
    this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_VID, buffer + 10, 1, true);

    // Pack it into the class
    this->pid[0] = buffer[0];
    this->pid[1] = buffer[1];
    this->pid[2] = buffer[2];
    this->pid[3] = buffer[3];
    this->fid = (uint16_t)(buffer[4] | buffer[5] << 7);
    this->x_res = (uint16_t)(buffer[6] | buffer[7] << 8);
    this->y_res = (uint16_t)(buffer[8] | buffer[9] << 8);
    this->vid = buffer[10];

    printf("GT911 PID: %s\n", this->pid);
    printf("GT911 FID: %04x\n", this->fid);
    printf("GT911 X_RES: %04x\n", this->x_res);
    printf("GT911 Y_RES: %04x\n", this->y_res);
    printf("GT911 VID: %02x\n", this->vid);
#endif
}

/**
 * @public
 * @brief Reset the GT911
 */
void gt911::reset()
{
#if defined(TOUCH_ENABLED)
    gpio_set_dir(TOUCH_I2C_IRQ_PIN, GPIO_OUT);

    gpio_put(TOUCH_I2C_RST_PIN, 0);
    sleep_us(20);
    gpio_put(TOUCH_I2C_IRQ_PIN, 0);
    sleep_ms(50);
    gpio_put(TOUCH_I2C_RST_PIN, 1);
    sleep_ms(100);

    gpio_set_dir(TOUCH_I2C_IRQ_PIN, GPIO_IN);
#endif
}

/**
 * @public
 * @brief Get the X position of the touch
 * @param id Touch ID, which finger to get the position of (Default: 0)
 * @returns X position
 */
uint32_t gt911::getX(uint32_t id)
{
    uint32_t id_safe = id > 4 ? 4 : id;
    return this->report[id_safe].x;
}

/**
 * @public
 * @brief Get the Y position of the touch
 * @param id Touch ID, which finger to get the position of (Default: 0)
 * @returns Y position
 */
uint32_t gt911::getY(uint32_t id)
{
    uint32_t id_safe = id > 4 ? 4 : id;
    return this->report[id_safe].y;
}

/**
 * @public
 * @brief Get the number of fingers on the display
 * @returns Number of fingers
 */
uint32_t gt911::getFingers()
{
    return this->num_touches;
}

/**
 * @private
 * @brief Fetch the touch data
 */
void gt911::fetch()
{
    uint32_t bytes_written = 0;

    // Get the status register
    uint8_t status = 0;
    bytes_written += this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_STATUS, &status);

    // If the buffer is not ready, wait for it
    while((status & 0x80) == 0)
    {
        bytes_written += this->touchRead(GT911_ADDR, (uint16_t)GT911_REG_STATUS, &status);
    }

    // Fill in the status report
    this->buffer_status = (uint8_t)((status >> 7) & 0x01);
    this->large_detect = (uint8_t)((status >> 6) & 0x01);
    this->proximity_valid = (uint8_t)((status >> 5) & 0x01);
    this->have_key = (uint8_t)((status >> 4) & 0x01);
    this->num_touches = (uint8_t)(status & 0x0f);

    // Read the touch data
    uint16_t touch_addr[5] = { GT911_REG_T0, GT911_REG_T1, GT911_REG_T2, GT911_REG_T3, GT911_REG_T4 };

    for (uint32_t i = 0; i < 5; i++)
    {
        uint8_t data[7] = { 0 };
        bytes_written += this->touchRead(GT911_ADDR, touch_addr[i], data, 7, true);

        uint8_t tid = data[GT911_OFFSET_TID];
        uint16_t x = (uint16_t)((data[GT911_OFFSET_XH] << 8) | data[GT911_OFFSET_XL]);
        uint16_t y = (uint16_t)((data[GT911_OFFSET_YH] << 8) | data[GT911_OFFSET_YL]);
        uint16_t size = (uint16_t)((data[GT911_OFFSET_SH] << 8) | data[GT911_OFFSET_SL]);

        // Fill in the report
        this->report[i] = {
            .trackID = tid,
            .x = x,
            .y = y,
            .size = size
        };

        if (this->display_ptr != nullptr)
        {
            // Translate the X and Y to the display
            uint32_t rotation = this->display_ptr->getRotation();

            switch (rotation)
            {
                case 0:
                    break;
                case 90:
                    std::swap(this->report[i].x, this->report[i].y);
                    this->report[i].y = this->display_ptr->getHeight() - this->report[i].y;
                    break;
                case 180:
                    this->report[i].x = this->display_ptr->getWidth() - this->report[i].x;
                    this->report[i].y = this->display_ptr->getHeight() - this->report[i].y;
                    break;
                case 270:
                    std::swap(this->report[i].x, this->report[i].y);
                    this->report[i].x = this->display_ptr->getHeight() - this->report[i].x;
                    break;
                default:
                    break;
            }
        }
    }

    this->available = true;
}