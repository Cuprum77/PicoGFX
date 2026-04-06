#include "uc8151.h"
#if defined(LCD_DRIVER_UC8151)

void uc8151::init()
{    
    this->maxWidth = MAX_WIDTH;
    this->maxHeight = MAX_HEIGHT;

    // Hard Reset
    this->hw->reset(10);

    // Panel Setting (PSR) - Keep a default standard mapping
    this->writeData(0x00, (const uint8_t *)"\x07", 1); 

    // Power Setting (PWR)
    this->writeData(0x01, (const uint8_t *)"\x03\x00\x2b\x2b\x2b", 5);

    // Power ON
    this->writeData(0x04, NULL, 0);

    while(this->hw->is_busy(true))
        sleep_ms(1);

    // Booster Soft Start (BTST)
    this->writeData(0x06, (const uint8_t *)"\x17\x17\x17", 3);

    // Panel Frames Setting (PFS)
    this->writeData(0x03, (const uint8_t *)"\x00", 1);

    // Temperature Sensor Enable (TSE)
    this->writeData(0x41, (const uint8_t *)"\x00", 1);

    // TCON Setting
    this->writeData(0x60, (const uint8_t *)"\x22", 1);

    // VCOM and Data Interval (CDI)
    this->writeData(0x50, (const uint8_t *)"\x97", 1);

    // PLL Control (Sets frequency to 100Hz)
    this->writeData(0x30, (const uint8_t *)"\x3c", 1);

    // Set Resolution (Physical resolution of the glass)
    // Assuming MAX_WIDTH = 128 and MAX_HEIGHT = 296 (adjust if yours is different)
    // 0x80, 0x01 (296) and 0x80 (128)
    this->writeData(0x61, (const uint8_t *)"\x80\x01\x80", 3);

    // Power OFF
    this->writeData(0x02, NULL, 0);

    while(this->hw->is_busy(true))
        sleep_ms(1);

    // Set initial rotation
    this->set_rotation(this->rotation);
}

/**
 * @brief Trigger a visual screen refresh
 */
void uc8151::refresh()
{
    this->writeData(0x12, NULL, 0);

    while(this->hw->is_busy(true))
        sleep_ms(10);
}

/**
 * @brief Set the software rotation of the display
 */
void uc8151::set_rotation(uint32_t rotation)
{
    this->rotation = rotation;

    // We swap the logical width and height for the API, 
    // but we leave the hardware registers alone.
    if (rotation == 90 || rotation == 270) 
    {
        this->width = MAX_HEIGHT;
        this->height = MAX_WIDTH;
    } 
    else 
    {
        this->width = MAX_WIDTH;
        this->height = MAX_HEIGHT;
    }
}

/**
 * @brief Trigger a visual screen refresh or turn off
 */
void uc8151::set_display_state(bool on)
{
    if (on) 
    {
        this->writeData(0x12, NULL, 0);
    } 
    else 
    {
        this->writeData(0x02, NULL, 0);
        
        const uint8_t deep_sleep_val = 0xa5;
        this->writeData(0x07, &deep_sleep_val, 1);
    }
}

/**
 * @private
 * @brief Write pixels to the display with rotation translation AND physical row mirroring
 * @param data Source framebuffer (formatted to match current rotation)
 * @param length Total number of pixels
 */
void uc8151::writePixels(const color_t *data, size_t length)
{
    // Wait until controller is idle
    while (this->hw->is_busy(true))
        sleep_ms(1);

    // Power ON 
    this->writeData(0x04, NULL, 0); 
    while (this->hw->is_busy(true))
        sleep_ms(1);

    // Data Start Transmission 2 (DTM2)
    this->writeData(0x13, NULL, 0);
    this->hw->setDataMode(0x13);

    // Rotate and mirror the image in software because the driver is fucking retarded
    for (uint32_t y = 0; y < MAX_HEIGHT; y++) 
    {
        for (uint32_t x = 0; x < MAX_WIDTH; x++) 
        {
            uint32_t src_idx = 0;

            switch (this->rotation) 
            {
                case 0:
                    src_idx = y * MAX_WIDTH + x;
                    break;
                case 90:
                    src_idx = (MAX_WIDTH - 1 - x) * MAX_HEIGHT + y;
                    break;
                case 180:
                    src_idx = (MAX_HEIGHT - 1 - y) * MAX_WIDTH + (MAX_WIDTH - 1 - x);
                    break;
                case 270:
                    src_idx = x * MAX_HEIGHT + (MAX_HEIGHT - 1 - y);
                    break;
            }

            uint32_t chip_x = MAX_WIDTH - 1 - x;
            this->secound_framebuffer[y * MAX_WIDTH + chip_x] = data[src_idx];
        }
    }
    
    // Send translated and flipped buffer to chip
    this->hw->writePixels(this->secound_framebuffer, length);

    // Data Stop (DSP)
    this->writeData(0x11, NULL, 0);

    // Display Refresh (DRF)
    this->writeData(0x12, NULL, 0);

    // Wait until the screen finishes updating
    while (this->hw->is_busy(true))
        sleep_ms(1);

    // Power OFF
    this->writeData(0x02, NULL, 0); 
}
#endif