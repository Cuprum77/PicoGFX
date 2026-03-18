#pragma once
#include "lcd_config.h"
#if defined(TOUCH_DRIVER_CST816)

#include "touch.h"
#include "display.h"

#define CST816_ADDR 0x15

typedef enum
{
    CST816_GESTURE_NONE         = 0x00,
    CST816_GESTURE_SWIPE_UP     = 0x01,
    CST816_GESTURE_SWIPE_DOWN   = 0x02,
    CST816_GESTURE_SWIPE_LEFT   = 0x03,
    CST816_GESTURE_SWIPE_RIGHT  = 0x04,
    CST816_GESTURE_SINGLE_CLICK = 0x05,
    CST816_GESTURE_DOUBLE_CLICK = 0x0b,
    CST816_GESTURE_LONG_PRESS   = 0x0c
} cst816_gesture_t;

typedef struct
{
    uint8_t gesture;
    uint8_t points;
    uint16_t x;
    uint16_t y;
} cst816_report_t;

class cst816 : public touch
{
public:
    cst816(display_obj *display = nullptr);
    void init();
    void reset();
    void disableStandby();
    void enableStandby();

    uint32_t getX();
    uint32_t getY();

    cst816_gesture_t getGesture() { return (cst816_gesture_t)this->report.gesture; }

private:
    uint32_t rst_pin;
    uint32_t irq_pin;
    cst816_report_t report;
    display_obj *display_ptr;

    static cst816 *instance;

    static void static_irq_handler(uint gpio, uint32_t event_mask)
    {
        if (instance != nullptr)
        {
            instance->fetch();
        }
    }

    void fetch();
    uint32_t getVersion();
    void getVersionValue(uint32_t *data);
};
#endif