#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.h"
#include "graphics.h"
#include "cst816.h"

// Display pins
#define DISP_PIN_DC         16
#define DISP_PIN_CS         17
#define DISP_PIN_SCK        18
#define DISP_PIN_MOSI       19
#define DISP_PIN_RST        20
#define DISP_PIN_BL         21

// Display constants
#define DISP_WIDTH          240     // 172 pixels wide
#define DISP_HEIGHT         280     // 320 p8ixels tall
#define DISP_OFFSET_X0      0		// cut off on the left side of the display
#define DISP_OFFSET_X1      0		// cut off on the right side of the display
#define DISP_OFFSET_Y0      20		// cut off on the top of the display
#define DISP_OFFSET_Y1      20		// cut off on the bottom of the display
#define DISP_ROTATION       display_rotation_t::ROTATION_0

// Touch pins
#define TOUCH_PIN_SDA       12
#define TOUCH_PIN_SCL       13
#define TOUCH_PIN_IRQ       14
#define TOUCH_PIN_RST       15

// set the display parameters
display_spi_config_t spi_config {
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.spi_instance = spi0,
	.baudrate = 125000000,
};

display_config_t config = {
	.backlightPin = DISP_PIN_BL,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION,
	.spi = spi_config
};

display_touch_config_t i2c_config {
    .i2c_inst = i2c0,
    .speed = 400'000,
    .sda_pin = TOUCH_PIN_SDA,
    .scl_pin = TOUCH_PIN_SCL,
    .irq_pin = TOUCH_PIN_IRQ,
    .rst_pin = TOUCH_PIN_RST
};

// Create the display object
hardware_driver hw(&config);
st7789 display(&hw, &config);
graphics draw(display.getFrameBuffer(), &config);
cst816 touch(&i2c_config, &display);

int main()
{
    // Initialize the Pico C SDK
    stdio_init_all();

    // init the built in led
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // Initialize the display
    hw.init();
    display.init();
    draw.fill(colors::white);

    touch.init();
    touch.disableStandby();
    
    display.update();

    uint64_t old_time = 0;

    while (1)
    {
        // blink the LED
        if (time_us_64() - old_time > 500'000)
        {
            old_time = time_us_64();
            gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
        }

        if (touch.ready())
        {
            point p = point(touch.getX(), touch.getY());
            static point op = display.getCenter();

            uint32_t circle_radius = 30;
            uint32_t circle_padding = circle_radius + 1;
            uint32_t circle_thickness = 5;

            p.x = imax(p.x, circle_padding);
            p.x = imin(p.x, display.getWidth() - circle_padding);

            p.y = imax(p.y, circle_padding);
            p.y = imin(p.y, display.getHeight() - circle_padding);

            draw.drawFilledCircle(op, circle_radius + (circle_thickness * 2), colors::white);
            draw.drawFilledCircleWithStroke(p, circle_radius, colors::pink, colors::black, circle_thickness);
            display.update();
            op = p;
        }
    }
    

    return 0;
}
