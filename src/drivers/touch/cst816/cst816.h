#pragma once
#include "lcd_config.h"
#if defined(TOUCH_DRIVER_CST816)

#include "touch.h"
#include "display.h"

#define CST816_ADDR 0x15

typedef enum
{
    CST816_GESTURE_NONE,
    CST816_GESTURE_SWIPE_UP,
    CST816_GESTURE_SWIPE_DOWN,
    CST816_GESTURE_SWIPE_LEFT,
    CST816_GESTURE_SWIPE_RIGHT,
    CST816_GESTURE_SINGLE_CLICK,
    CST816_GESTURE_DOUBLE_CLICK,
    CST816_GESTURE_LONG_PRESS
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