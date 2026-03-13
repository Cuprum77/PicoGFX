#pragma once

#include "touch.h"
#include "display.h"

#define GT911_ADDR              0x5d

#define GT911_REG_CMD           0x8040  // Config register
#define GT911_REG_TOUCH_NUM     0x804c  // Number of touches
#define GT911_REG_STATUS        0x814e  // Status register
#define GT911_REG_PID           0x8140  // Product ID
#define GT911_REG_FID           0x8144  // Firmware ID
#define GT911_REG_X_RES         0x8146  // X resolution
#define GT911_REG_Y_RES         0x8148  // Y resolution
#define GT911_REG_VID           0x814a  // Vendor ID

#define GT911_REG_T0            0x814f  // Touch 0
#define GT911_REG_T1            0x8157  // Touch 1
#define GT911_REG_T2            0x815f  // Touch 2
#define GT911_REG_T3            0x8167  // Touch 3
#define GT911_REG_T4            0x816f  // Touch 4

#define GT911_OFFSET_TID    0x00  // Touch ID offset
#define GT911_OFFSET_XH     0x01  // X high offset
#define GT911_OFFSET_XL     0x02  // X low offset
#define GT911_OFFSET_YH     0x03  // Y high offset
#define GT911_OFFSET_YL     0x04  // Y low offset
#define GT911_OFFSET_SH     0x05  // Size high offset
#define GT911_OFFSET_SL     0x06  // Size low offset

typedef struct
{
    uint8_t trackID;
    uint16_t x;
    uint16_t y;
    uint16_t size;
} gt911_report_t;

class gt911 : public touch
{
public:
    gt911(display *display_ptr = nullptr);
    void init();
    void reset();

    uint32_t getX(uint32_t id = 0);
    uint32_t getY(uint32_t id = 0);
    uint32_t getFingers();

private:
    display *display_ptr;
    gt911_report_t report[5] = { 0 };

    uint8_t buffer_status;
    uint8_t large_detect;
    uint8_t proximity_valid;
    uint8_t have_key;
    uint8_t num_touches;

    char pid[4] = "";
    uint32_t fid = 0;
    uint32_t vid = 0;
    uint32_t x_res = 0;
    uint32_t y_res = 0;

    static gt911 *instance;

    static void static_irq_handler(uint gpio, uint32_t event_mask)
    {
        if (instance != nullptr)
        {
            instance->fetch();
        }
    }

    void fetch();
};